#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 outTexCoord;
layout(location = 1) out vec3 outWorldPos;
layout(location = 2) out float outTransparency;

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

struct Particle{
    vec4 position;
    vec4 rotation_transparency;
};

layout(set = 1, binding = 0) buffer modelStorageBuffer {
    ivec4 particlesCount;
    Particle particles[];
} modelBuffer;

#include rotmat3d.glsl

void main() {
    uint vid = gl_VertexIndex;
    Particle p = modelBuffer.particles[gl_InstanceIndex];
    vec3 pos = p.position.rgb;
    float size = p.position.a;
    float rot = p.rotation_transparency.x;
    outTransparency = p.rotation_transparency.y;
    vec3 displacements[4] = vec3[](
        -hiFreq.inFrustumConeBottomLeftToBottomRight.rgb,
        -hiFreq.inFrustumConeBottomLeftToTopLeft.rgb,
        hiFreq.inFrustumConeBottomLeftToBottomRight.rgb,
        hiFreq.inFrustumConeBottomLeftToTopLeft.rgb
    );
    vec2 UVs[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    );
    mat3 mat = rotationMatrix(normalize(pos), rot);
    vec2 txn = (inTexCoord * 2.0 - 1.0);
    vec3 vector = txn.x * normalize(hiFreq.inFrustumConeBottomLeftToBottomRight.rgb)
                      + txn.y * normalize(hiFreq.inFrustumConeBottomLeftToTopLeft.rgb);
    vec3 displacement = mat * vector;
    pos += displacement * size;

    vec3 WorldPos = pos;
    outWorldPos = WorldPos;
    vec4 opo = (hiFreq.VPMatrix)
        * vec4(WorldPos, 1.0);
    outTexCoord = vec2(inTexCoord.x, 1.0 - inTexCoord.y);
    opo.y *= -1.0;
    gl_Position = opo;
}
