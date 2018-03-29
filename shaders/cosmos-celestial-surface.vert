#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 outDir;
layout(location = 1) out flat uint inInstanceId;
layout(location = 2) out vec3 outWorldPos;
layout(location = 3) out vec3 outNormal;

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
    vec3 dir = inPosition.xyz;
    float surfaceHeight = texture(heightMapImage, xyzToPolar(dir)).r;
    vec3 WorldPos = (inverse(body.rotationMatrix) * dir) * (body.radius + body.terrainMaxLevel * surfaceHeight) + body.position;
    vec4 opo = (hiFreq.VPMatrix) * vec4(WorldPos, 1.0);
    vec3 Normal = dir;
    outNormal = dir;//celestialGetNormal(body, 0.001, dir);
    outDir = dir;
    outWorldPos = WorldPos;
    opo.y *= -1.0;
    gl_Position = opo;
}
