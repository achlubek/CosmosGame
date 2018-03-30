#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texCelestialAlpha;
layout(set = 0, binding = 2) uniform sampler2D texStars;
layout(set = 0, binding = 3) uniform sampler2D uiTexture;
layout(set = 0, binding = 4) uniform sampler2D texShip;
layout(set = 0, binding = 5) uniform sampler2D texCelestialAdditive;

#include rendererDataSet.glsl

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * hiFreq.Time,vec2(12.9898,78.233))) * 43758.5453);
}
float rand2s2(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x) {
   return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}
vec3 tonemapUncharted2(vec3 color) {
    float ExposureBias = 2.0;
    vec3 curr = Uncharted2Tonemap(ExposureBias * color);

    vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
    return curr * whiteScale;
}

vec3 gammacorrect(vec3 c){
    return pow(c, vec3(1.0 / 2.4));
}

#include camera.glsl
void main() {
    vec4 celestial = texture(texCelestialAlpha, UV);
    vec3 dir = reconstructCameraSpaceDistance(gl_FragCoord.xy / Resolution, 1.0);

    vec3 stars = texture(texStars, UV).rgb ;//texture(texStars, UV);
    vec4 ui = texture(uiTexture, UV);
    //stars.rgb /= max(0.0001, stars.a);
    vec3 a = celestial.rgb;//mix(stars, celestial.rgb, celestial.a);
    vec4 adddata = texture(texCelestialAdditive, UV).rgba;

    vec3 starDir = normalize(-ClosestStarPosition + vec3(0.000001));
    vec3 sunflare = exp(-2000.0 * (dot(dir, starDir) * 0.5 + 0.5)) * ClosestStarColor * 0.1;
    sunflare += exp(-200.0 * (dot(dir, starDir) * 0.5 + 0.5)) * ClosestStarColor * 0.01;
    sunflare += exp(-20.0 * (dot(dir, starDir) * 0.5 + 0.5)) * ClosestStarColor * 0.001;
    sunflare = pow(1.0 - (dot(dir, starDir) * 0.5 + 0.5), 62.0) * ClosestStarColor * 0.1 * max(0.0, 1.0 - adddata.a);
    //sunflare += pow(1.0 - (dot(dir, starDir) * 0.5 + 0.5), 62.0) * ClosestStarColor * 0.01;

    a += adddata.rgb + sunflare * 0.0001;
    vec4 shipdata = texture(texShip, UV).rgba;
    a = mix(a, shipdata.rgb, shipdata.a);
    a = mix(a, ui.rgb, ui.a);
    outColor = vec4(gammacorrect(clamp(a, 0.0, 10000.0)), 1.0);
}
