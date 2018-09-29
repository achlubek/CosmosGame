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


#include rendererDataSet.glsl

layout(set = 1, binding = 0) buffer modelStorageBuffer {
    mat4 transformation;
    vec4 position;
    ivec4 id;
    vec4 emissionvalue;
} modelBuffer;

#include shadowMapDataSet.glsl

void main() {
    vec3 WorldPos = (modelBuffer.transformation
        * vec4(inPosition.xyz, 1.0)).rgb * 1.0 * modelBuffer.position.a + modelBuffer.position.rgb;
    vec4 opo = vec4(mat3(FromStarToThisMatrix) * (WorldPos / Divisor), 1.0);
    opo.z =clamp(opo.z, -0.999, 0.999);
    outDepth = opo.z * 0.5 + 0.5;
    gl_Position = opo;
}
