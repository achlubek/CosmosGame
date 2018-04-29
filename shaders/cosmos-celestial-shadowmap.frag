#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Dir;
layout(location = 1) in flat uint inInstanceId;
layout(location = 2) in float inDepth;
layout(location = 3) in vec3 inNormal;
layout(location = 0) out float outDistance;

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
#include shadowMapDataSet.glsl

float celestialGetHeight(vec3 dir){
    return texture(heightMapImage, xyzToPolar(dir)).r;
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

    outDistance = 1.0 - inDepth;//1.0;//length(inWorldPos);
    //gl_FragDepth = inDepth;
}
