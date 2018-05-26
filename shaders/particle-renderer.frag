#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outResult;
layout(set = 1, binding = 1) uniform sampler2D texParticle;

layout(set = 1, binding = 0) buffer modelStorageBuffer {
    mat4 transformation;
    vec4 position;
    ivec4 id;
} modelBuffer;

#include rendererDataSet.glsl

#include camera.glsl
#include sphereRaytracing.glsl
#include polar.glsl
#include transparencyLinkedList.glsl
#include rotmat3d.glsl

Ray cameraRay;
void main() {
    outResult = texture(texParticle, UV).rgba;
}
