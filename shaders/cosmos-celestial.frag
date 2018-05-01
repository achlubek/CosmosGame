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

vec3 extra_cheap_atmosphere(float raylen, float sunraylen, float absorbstrength, vec3 absorbcolor, float sunraydot){
    //sundir.y = max(sundir.y, -0.07);
    sunraydot = max(0.0, sunraydot);
    raylen *= absorbstrength * 0.004;
    sunraylen *= absorbstrength * 0.004;
    sunraylen = min(sunraylen, 1.8);
    float raysundt = pow(abs(sunraydot), 2.0);
    float sundt = pow(max(0.0, sunraydot), 32.0);
    float mymie = sundt * raylen;
    vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - absorbcolor), clamp(sunraylen, 0.0, 1.0)) / (1.0 + raylen);
    vec3 bluesky= absorbcolor * suncolor;
    vec3 bluesky2 = max(vec3(0.0), bluesky - absorbcolor * 0.08 * (raylen));
    bluesky2 *= raylen * (0.24 + raysundt * 0.24);
    return bluesky2 + mymie * suncolor;
}

void main() {
    RenderedCelestialBody body = getRenderedBody(celestialBuffer.celestialBody);
    vec3 dir = reconstructCameraSpaceDistance(gl_FragCoord.xy / Resolution, 1.0);
    CelestialRenderResult result = renderCelestialBody(body, Ray(vec3(0.0), dir));
    result.alphaBlendedLight.rgb *= Exposure;
    result.additionLight.rgb *= Exposure;
    outColorAlpha = result.alphaBlendedLight;
    outColorAdditive = vec4(result.additionLight.rgb, result.additionLight.a);

//    outColorAlpha = vec4(getShadowAtPoint(body, dir * texture(surfaceRenderedDistanceImage, gl_FragCoord.xy / Resolution).r) * vec3(1.0), result.alphaBlendedLight.a);//result.alphaBlendedLight;
    //outColorAdditive = 0.0*result.additionLight;
}
