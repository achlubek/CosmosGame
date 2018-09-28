#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 Dir;
layout(location = 1) in flat uint inInstanceId;
layout(location = 2) in float inDepth;
layout(location = 3) in vec3 inNormal;
layout(location = 0) out float outDistance;

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

    outDistance = 1.0 - inDepth;//1.0;//length(inWorldPos);
    //gl_FragDepth = inDepth;
}
