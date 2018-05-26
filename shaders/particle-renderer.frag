#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 1) in vec3 inWorldPos;
layout(location = 2) in float inTransparency;

layout(location = 0) out vec4 outResult;

layout(set = 0, binding = 1) uniform sampler2D texDistance;
layout(set = 1, binding = 1) uniform sampler2D texParticle;

#include rendererDataSet.glsl

#include camera.glsl
#include sphereRaytracing.glsl
#include polar.glsl
#include transparencyLinkedList.glsl
#include rotmat3d.glsl

struct Particle{
    vec4 position;
    vec4 rotation_transparency;
};

layout(set = 1, binding = 0) buffer modelStorageBuffer {
    ivec4 particlesCount;
    Particle particles[];
} modelBuffer;

Ray cameraRay;
void main() {
    float dist = distance(CameraPosition, inWorldPos);
    float test = texture(texDistance, UV).r;
    vec4 tdata = texture(texParticle, UV).rgba;
    outResult = tdata * vec4(vec3(inTransparency * tdata.a), 1.0);
}
