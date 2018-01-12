#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out flat uint inInstanceId;
layout(location = 2) out vec3 outWorldPos;
layout(location = 3) out vec3 outNormal;

#include celestialSet.glsl

layout(set = 1, binding = 0) buffer modelStorageBuffer {
    mat4 transformation;
    vec4 position;
} modelBuffer;

void main() {
    vec3 WorldPos = (modelBuffer.transformation
        * vec4(inPosition.xyz * modelBuffer.position.a, 1.0)).rgb + modelBuffer.position.rgb;
    vec4 opo = (hiFreq.VPMatrix)
        * ((modelBuffer.transformation * vec4(inPosition.xyz * modelBuffer.position.a, 1.0)) + vec4(modelBuffer.position.rgb, 0.0));
    vec3 Normal = inNormal;
    outNormal = normalize(Normal);
    outTexCoord = inTexCoord;
    outWorldPos = WorldPos;
    opo.y *= -1.0;
    gl_Position = opo;
}
