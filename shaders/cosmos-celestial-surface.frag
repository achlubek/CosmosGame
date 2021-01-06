#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Dir;
layout(location = 1) in flat uint inInstanceId;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inNormal;
layout(location = 0) out vec4 outAlbedoRoughness;
layout(location = 1) out vec4 outEmission;
layout(location = 2) out vec4 outNormalMetalness;
layout(location = 3) out float outDistance;

#include rendererDataSet.glsl
#include sphereRaytracing.glsl
#include proceduralValueNoise.glsl
#include wavesNoise.glsl
#include celestialDataStructs.glsl
#include celestialRenderSurfaceSet.glsl
#include polar.glsl
#include rotmat3d.glsl
#include textureBicubic.glsl
#include camera.glsl


float getwavesHighPhaseTerrainRefinement(vec3 position, float dragmult, float timeshift, float seed){
    float iter = 0.0;
    float seedWaves = seed;
    float phase = 2.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<15;i++){
        vec3 p = (vec3(oct(seedWaves += 1.0), oct(seedWaves += 1.0), oct(seedWaves += 1.0)) * 2.0 - 1.0) * 300.0;
        float res = wave(position, p, speed, phase * 20.0, 0.0 + timeshift);
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.1;
        speed *= 1.02;
    }
    return w / ws;
}
float celestialGetHeight(vec3 direction){
    float primary = textureBicubic(heightMapImage, xyzToPolar(direction)).r;
    float secondary = abs(FBM3(direction * 220.9, 5, 3.0, 0.55) - 0.5);
    float refinement = pow(getwavesHighPhaseTerrainRefinement(direction.xyz * 10.1, 1.0, 0.0, 0.0),1.0);
    //return primary * 30.0 + secondary;
    vec3 coord = normalize(direction) * 10.0;
    //return 0.0;
    return primary * 0.98 + secondary * 0.02;//smoothstep(0.99, 0.999, primary);
    //return texture(heightMapImage, xyzToPolar(dir)).r;
}

vec3 celestialGetNormal(RenderedCelestialBody body, float dxrange, vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, dxrange);
    mat3 normrotmat2 = rotationMatrix(bitangdir, dxrange);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * (body.radius + celestialGetHeight(dir) * body.terrainMaxLevel);
    vec3 p2 = dir2 * (body.radius + celestialGetHeight(dir2) * body.terrainMaxLevel);
    vec3 p3 = dir3 * (body.radius + celestialGetHeight(dir3) * body.terrainMaxLevel);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}

void main() {

    RenderedCelestialBody body = getRenderedBody(celestialBuffer.celestialBody);

    outAlbedoRoughness = texture(baseColorImage, xyzToPolar(Dir)).rgba;
    outNormalMetalness = vec4(inverse(body.rotationMatrix) * celestialGetNormal(body, 0.001, Dir).rgb, 0.0);
    outDistance = length(inWorldPos);
    outEmission = vec4(0.0);
    float C = 0.001;
    float w = length(inWorldPos);
    float Far = 10000.0;
    gl_FragDepth = min(1.0, log(C*w + 1.0) / log(C*Far + 1.0));
}
