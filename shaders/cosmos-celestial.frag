#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColorAlpha;
layout(location = 1) out vec4 outColorAdditive;


layout(set = 2, binding = 0) uniform sampler2D shadowMap1;
layout(set = 2, binding = 1) uniform sampler2D shadowMap2;
layout(set = 2, binding = 2) uniform sampler2D shadowMap3;

#include rendererDataSet.glsl
#include sphereRaytracing.glsl
#include proceduralValueNoise.glsl
#include wavesNoise.glsl
#include celestialDataStructs.glsl
#include celestialRenderSet.glsl
#include polar.glsl
#include rotmat3d.glsl
#include textureBicubic.glsl
#include celestialCommons.glsl
#include camera.glsl

void main() {
    RenderedCelestialBody body = getRenderedBody(celestialBuffer.celestialBody);
    vec3 dir = reconstructCameraSpaceDistance(gl_FragCoord.xy / Resolution, 1.0);
    CelestialRenderResult result = renderCelestialBody(body, Ray(vec3(0.0), dir));
    result.alphaBlendedLight.rgb *= Exposure;
    result.additionLight.rgb *= Exposure;
    outColorAlpha = result.alphaBlendedLight;
    outColorAdditive = result.additionLight;

//    outColorAlpha = vec4(getShadowAtPoint(body, dir * texture(surfaceRenderedDistanceImage, gl_FragCoord.xy / Resolution).r) * vec3(1.0), result.alphaBlendedLight.a);//result.alphaBlendedLight;
    //outColorAdditive = 0.0*result.additionLight;
}
