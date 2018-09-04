#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out flat uint inInstanceId;

#include rendererDataSet.glsl

struct GeneratedStarInfo {
    vec4 position_radius;
    vec4 color_zero; //0->maybe 10? maybe 100?
};

layout(set = 1, binding = 0) buffer StarsStorageBuffer {
    GeneratedStarInfo stars[];
} starsBuffer;

void main() {
    inInstanceId = gl_InstanceIndex;

    // get star data
    vec3 starPosition = starsBuffer.stars[gl_InstanceIndex].position_radius.xyz;
    float starRadius = starsBuffer.stars[gl_InstanceIndex].position_radius.a;

    // transform star position into camera space
    starPosition -= CameraPosition;

    // calculate real distance and clamp it to avoid invisible stars
    float dist = min(250000.0, length(starPosition));

    // calculate camera space position of the vertex, 2.0 multiplier to avoid bugs at edges
    vec3 cameraSpacePos = normalize(starPosition) * dist + inPosition.xyz * starRadius * 2.0;

    // project the vertex
    vec4 outPos = (hiFreq.VPMatrix) * vec4(cameraSpacePos, 1.0);

    outPos.y *= -1.0;
    vec2 newuv = (outPos.xy / outPos.w) * 0.5 + 0.5;
    gl_Position = outPos;
}
