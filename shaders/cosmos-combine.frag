#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texCelestialAlpha;
layout(set = 0, binding = 2) uniform sampler2D texStars;
layout(set = 0, binding = 3) uniform sampler2D texCelestialAdditive;
layout(set = 0, binding = 4) uniform sampler2D texModelsAlbedoRoughness;
layout(set = 0, binding = 5) uniform sampler2D texModelsNormalMetalness;
layout(set = 0, binding = 6) uniform sampler2D texModelsDistance;
layout(set = 0, binding = 7) uniform sampler2D texParticlesResult;

#include rendererDataSet.glsl
#include proceduralValueNoise.glsl
#include sphereRaytracing.glsl

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

vec3 afl_tonemap(vec3 c){
    /*
    vec3 underExposed = c * 0.1;
    vec3 mediumExposed = c * 1.1;
    vec3 overExposed = c * 10.1;
    float exposure = 0.0;//textureBicubic(c)
    float divisor = 0.0;
    vec3 pixel = vec3(1.0 / Resolution, 0.0);
    vec3 bloom = vec3(0.0);
    float lengthLimit = length(vec2(4.0, 4.0));
    for(int x=-4;x<4;x++){
        for(int y=-4;y<4;y++){
            vec2 displacer = pixel.xy * vec2(x, y) * (1.0 - smoothstep(0.0, lengthLimit, length(vec2(x, y))));
            vec4 celestial = texture(texCelestialAlpha, UV + displacer).rgba;
            vec3 adddata = texture(texCelestialAdditive, UV + displacer).rgb;
            exposure += length(celestial.rgb * celestial.a + adddata);
            bloom += (celestial.rgb * celestial.a + adddata);
            divisor += 1.0;
        }
    }
    exposure /= divisor;
    bloom /= divisor;
    c += bloom;*/
    vec3 bleachedShadows = pow(mix(c, vec3(length(c)), 0.17), vec3(2.0)) * 1.0;
    vec3 dimmedIntensiveLight = c * 0.07;
    vec3 result = normalize(c) * pow(length(c), 0.5);
    float L = 0.27*c.r + 0.67*c.g + 0.06*c.b;
    float B = 0.01*c.r + 0.47*c.g + 0.52*c.b;
    float Ld = L / ( 0.01 + L * 5.0 );
    vec3 bleachx = vec3(B) * vec3(0.01, 0.47, 0.52) * 0.02;
    //return gammacorrect(bleachx);
    return gammacorrect(Ld * c + bleachx);//gammacorrect(normalize(c) * 0.1 * pow(exposure, 0.7));
}

vec2 project(vec3 pos){
    vec4 tmp = (hiFreq.VPMatrix * vec4(pos, 1.0));
    return (tmp.xy / tmp.w) * 0.5 + 0.5;
}
mat3 ACESInputMat = mat3(
    0.59719, 0.35458, 0.04823,
    0.07600, 0.90834, 0.01566,
    0.02840, 0.13383, 0.83777
);

mat3 ACESOutputMat = mat3(
     1.60475, -0.53108, -0.07367,
    -0.10208,  1.10813, -0.00605,
    -0.00327, -0.07276,  1.07602
);

vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return a / b;
}

vec3 ACESFitted(vec3 color)
{
    color = transpose(ACESInputMat) * color;
    color = RRTAndODTFit(color);
    color = (transpose(ACESOutputMat) * color);
    return gammacorrect(clamp(color, 0.0, 1.0));
}
#include camera.glsl
#include pbr.glsl
void main() {
    vec4 celestial = texture(texCelestialAlpha, UV);
    vec3 dir = reconstructCameraSpaceDistance(gl_FragCoord.xy / Resolution, 1.0);

    vec3 stars = texture(texStars, UV).rgb ;//texture(texStars, UV);
    //stars.rgb /= max(0.0001, stars.a);
    vec3 a = celestial.rgb;//mix(stars, celestial.rgb, celestial.a);
    vec4 adddata = texture(texCelestialAdditive, UV).rgba;

    vec3 starDir = normalize(-ClosestStarPosition + vec3(0.000001));
    float starDist = length(ClosestStarPosition);
    float starhit = rsi2(Ray(vec3(0.0), dir), Sphere( starDist * -starDir, 55.0)).y;
    vec3 sunflare = exp(starDist * -2000.0 * (dot(dir, starDir) * 0.5 + 0.5)) * ClosestStarColor * 0.1;
    sunflare += exp(starDist * -200.0 * (dot(dir, starDir) * 0.5 + 0.5)) * ClosestStarColor * 0.01;
    sunflare += exp(starDist * -20.0 * (dot(dir, starDir) * 0.5 + 0.5)) * ClosestStarColor * 0.001;
    vec2 displaceVector = normalize(project(dir) - project(starDir)) * 10.0;
    float flunctuations = 0.3 + 0.7 * smoothstep(0.2, 0.7, noise3d(vec3(displaceVector, Time)));
    sunflare = 0.0* flunctuations * pow(1.0 - (dot(dir, starDir) * 0.5 + 0.5), starDist * 0.08) * ClosestStarColor * 0.02 * max(0.0, 1.0 - adddata.a);
    sunflare += exp(starDist * -0.0025 * (dot(dir, starDir) * 0.5 + 0.5)) * ClosestStarColor *3.3 * max(0.0, 1.0 - adddata.a);
    //sunflare += pow(1.0 - (dot(dir, starDir) * 0.5 + 0.5), 62.0) * ClosestStarColor * 0.01;
    vec3 sunFlareColorizer = mix(vec3(1.0), normalize(adddata.rgb + 0.001), min(1.0, 1010.0 *length(adddata.rgb)));
    vec3 stnorm = normalize(dir * starhit - starDist * -starDir);
    float snois = (starhit > 0.0 && starhit < 9999999.0) ? (aBitBetterNoise(stnorm * 10.0) * 0.5 + 0.25 * aBitBetterNoise(stnorm * 30.0)) : 0.0;
    sunflare = ((starhit > 0.0 && starhit < 9999999.0) ? 1.0 : 0.0) * ClosestStarColor * max(0.0, 1.0 - adddata.a) * sunFlareColorizer * Exposure * 21.8 * snois;
    a += adddata.rgb + sunflare;
    vec4 shipdata1 = texture(texModelsAlbedoRoughness, UV).rgba;
    vec4 shipdata2 = texture(texModelsNormalMetalness, UV).rgba;
    float shipdata3 = texture(texModelsDistance, UV).r;
    vec3 albedo = shipdata1.rgb;
    float roughness = shipdata1.a;
    vec3 normal = normalize(shipdata2.rgb);
    float metalness = shipdata2.a;
    vec3 position = dir * shipdata3;
    vec3 viewdir = dir;
    vec3 lightdir = -starDir;
    vec3 lightcolor = ClosestStarColor * 0.0001;

    vec3 shaded = shade_ray(albedo, normal, viewdir, roughness, metalness, lightdir, lightcolor);

    a = mix(a, shaded, step(0.09, length(shipdata2.rgb)));
    vec4 particlesData = texture(texParticlesResult, UV).rgba;
    a += particlesData.a == 0.0 ? vec3(0.0) : (particlesData.rgb);
    outColor = vec4(ACESFitted(clamp(a, 0.0, 10000.0)), 1.0);
}
