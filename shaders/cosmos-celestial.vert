#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint inInstanceId;
layout(location = 2) out vec3 outWorldPos;

#include rendererDataSet.glsl
#include camera.glsl
#include sphereRaytracing.glsl
#include celestialDataStructs.glsl
#include celestialRenderSet.glsl

void main() {
    vec4 posradius = celestialBuffer.celestialBody.position_radius;
    float atmoradius = celestialBuffer.celestialBody.sufraceMainColor_atmosphereHeight.a;
    outWorldPos = posradius.xyz + inPosition.xyz * (posradius.a + atmoradius) * 40.0;

    vec4 opo = (hiFreq.VPMatrix) * vec4(outWorldPos, 1.0);
    opo.y *= -1.0;
    vec2 newuv = (opo.xy / opo.w) * 0.5 + 0.5;
    outTexCoord = newuv;
    gl_Position = opo; //    gl_Position = vec4(inPosition.xyz, 1.0);
}
