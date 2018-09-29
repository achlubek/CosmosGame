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
layout(location = 2) out float Depth;
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
#include shadowMapDataSet.glsl

void main() {
    RenderedCelestialBody body = getRenderedBody(celestialBuffer.celestialBody);
    vec3 dir = inPosition.xyz;
    float surfaceHeight = texture(heightMapImage, xyzToPolar(dir)).r;
    vec3 WorldPos = (inverse(body.rotationMatrix) * dir) * (body.radius + max(body.fluidMaxLevel, body.terrainMaxLevel * surfaceHeight)) + body.position;
    vec4 opo = vec4(mat3(FromStarToThisMatrix) * (WorldPos * Divisor), 1.0);
    opo.x = clamp(opo.x, -1.0, 1.0);
    opo.y = clamp(opo.y, -1.0, 1.0);
    opo.z = clamp(opo.z, -0.9999, 0.9999);
    vec3 Normal = dir;
    outNormal = dir;
    outDir = dir;
    opo.y *= -1.0;
    Depth = opo.z * 0.5 + 0.5;
    gl_Position = opo;
}
