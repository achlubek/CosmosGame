#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 1) in flat uint inInstanceId;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inNormal;
layout(location = 0) out vec4 outNormalMetalness;
layout(location = 1) out float outDistance;

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

void main() {

    RenderedCelestialBody body = getRenderedBody(celestialBuffer.celestialBody);

    outNormalMetalness = vec4(inNormal, 0.0);
    outDistance = length(inWorldPos) ;
    float C = 0.001;
    float w = length(inWorldPos);
    float Far = 1000.0;
    gl_FragDepth = min(1.0, log(C*w + 1.0) / log(C*Far + 1.0));
}
