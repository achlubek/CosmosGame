#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inTangent;

layout(location = 0) out float outDepth;


layout(set = 0, binding = 0) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
    vec4 inCameraPos;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
    vec4 Resolution_scale;
} hiFreq;

layout(set = 0, binding = 1) uniform sunLightDataBuffer {
    mat4 SunLightDirection_Zero;
} sunLightData;

layout(set = 1, binding = 0) buffer modelStorageBuffer {
    mat4 transformation;
    vec4 position;
    ivec4 id;
    vec4 emissionvalue;
} modelBuffer;

void main() {
    vec3 WorldPos = (modelBuffer.transformation
        * vec4(inPosition.xyz, 1.0)).rgb * 1.0 * modelBuffer.position.a + modelBuffer.position.rgb;
    vec4 opo = (sunLightData.SunLightDirection_Zero)
        * vec4(WorldPos * 0.01, 1.0);
    opo.y *= -1.0;
    outDepth = opo.z * 0.5 + 0.5;
    gl_Position = opo;
}
