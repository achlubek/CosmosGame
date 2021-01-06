#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

#include rendererDataSet.glsl


layout(set = 0, binding = 1) uniform sampler2D texCelestialAlpha;
layout(set = 0, binding = 2) uniform sampler2D texStars;
layout(set = 0, binding = 3) uniform sampler2D texCelestialAdditive;

#include proceduralValueNoise.glsl
#include sphereRaytracing.glsl

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * hiFreq.Time,vec2(12.9898,78.233))) * 43758.5453);
}
float rand2s2(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec2 project(vec3 pos){
    vec4 tmp = (hiFreq.VPMatrix * vec4(pos, 1.0));
    return ((tmp.xy / tmp.w) * vec2(1.0, -1.0)) * 0.5 + 0.5;
}

vec3 aces_tonemap(vec3 color){
    mat3 m1 = mat3(
        0.59719, 0.07600, 0.02840,
        0.35458, 0.90834, 0.13383,
        0.04823, 0.01566, 0.83777
    );
    mat3 m2 = mat3(
        1.60475, -0.10208, -0.00327,
        -0.53108,  1.10813, -0.07276,
        -0.07367, -0.00605,  1.07602
    );
    vec3 v = m1 * color;
    vec3 a = v * (v + 0.0245786) - 0.000090537;
    vec3 b = v * (0.983729 * v + 0.4329510) + 0.238081;
    return pow(clamp(m2 * (a / b), 0.0, 1.0), vec3(1.0 / 2.2));
}

#include camera.glsl
#include pbr.glsl


#define chromaShift 0.5

float subsubflare(vec2 uv, vec2 point){
    float d = distance(uv, point) ;
    return step(d, 0.02) * (0.02-d)/0.02;
}

mat2 rotmat2d(float angle){
    return mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
}

vec3 subflare(vec2 uv, vec2 point, float px, float py, float pz, float cShift, float i)
{

    uv-=.5;
    float x = length(uv);
    uv *= pow(4.0*x,py)*px+pz;

    vec3 t=vec3(0.);
    t.r = subsubflare(clamp(uv*(1.0+cShift*chromaShift)+0.5, 0.0, 1.0), point);
    t.g = subsubflare(clamp(uv+0.5, 0.0, 1.0), point);
    t.b = subsubflare(clamp(uv*(1.0-cShift*chromaShift)+0.5, 0.0, 1.0), point);
    t = t*t;
    t *= clamp(.6-length(uv), 0.0, 1.0);
    t *= clamp(length(uv*20.0), 0.0, 1.0);
    t *= i;
    return t;
}

vec2 correctRatio(vec2 uv){
    return uv * vec2(1.0, Resolution.y / Resolution.x);
}

vec3 flare(vec2 point, vec2 uv){

    float d = max(0.000001, distance(correctRatio(uv), correctRatio(point)) - 0.01);
    float tt = 1.0 / abs( d * 55.0 );
    mat2 rm1 = rotmat2d(3.1415 * 0.25);
    mat2 rm2 = rotmat2d(3.1415 * 0.75);
    float v = 1.0 / abs( length((point-uv) * vec2(0.03, 1.0)) * (150.0) );
    float v2 = 1.0 / abs( length((point-uv) * vec2(1.0, 0.09)) * (750.0) );
    float v3 = 1.0 / abs( length((rm1*(point-uv)) * (vec2(1.0, 0.09))) * (1750.0) );
    float v4 = 1.0 / abs( length((rm2*(point-uv)) * (vec2(1.0, 0.09))) * (1750.0) );

    vec3 finalColor = vec3(subsubflare(uv, point)*0.5);
    finalColor += vec3(tt);
    finalColor += vec3( v );
    finalColor += vec3( v2 );
    finalColor += vec3( v3 );
    finalColor += vec3( v4 );

    finalColor += subflare(uv, point, 0.00005, 16.0, 0.0, 0.2, 2.0);
    finalColor += subflare(uv, point, 0.5, 2.0, 0.0, 0.1, 2.0);
    finalColor += subflare(uv, point, 20.0, 1.0, 0.0, 0.05, 2.0);
    finalColor += subflare(uv, point, -10.0, 1.0, 0.0, 0.1, 2.0);
    finalColor += subflare(uv, point, -10.0, 2.0, 0.0, 0.05, 4.0);
    finalColor += subflare(uv, point, -1.0, 1.0, 0.0, 0.1, 4.0);
    finalColor += subflare(uv, point, -0.00005, 16.0, 0.0, 0.2, 4.0);
    return finalColor;
}

void main() {
    vec4 celestial = texture(texCelestialAlpha, UV);
    vec3 dir = reconstructCameraSpaceDistance(gl_FragCoord.xy / Resolution, 1.0);

    vec3 stars = texture(texStars, UV).rgb ;//texture(texStars, UV);
    //stars.rgb /= max(0.0001, stars.a);
    vec3 a = mix(stars, celestial.rgb, celestial.a);
    vec4 adddata = texture(texCelestialAdditive, UV).rgba;

    vec3 starDir = normalize(-ClosestStarPosition + vec3(0.000001));
    float starDist = length(ClosestStarPosition);
    float starhit = rsi2(Ray(vec3(0.0), dir), Sphere( starDist * -starDir, 155.0)).y;
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

    vec2 projectedSunDir = project(starDir);
    vec4 adddata2 = texture(texCelestialAdditive, clamp(projectedSunDir, 0.0, 1.0)).rgba;
    //vec4 shipdata222 = texture(texModelsNormalMetalness, clamp(projectedSunDir, 0.0, 1.0)).rgba;
    //vec4 shipdata1 = texture(texModelsAlbedoRoughness, UV).rgba;
    //vec4 shipdata2 = texture(texModelsNormalMetalness, UV).rgba;

    //sunflare = ((starhit > 0.0 && starhit < 9999999.0) ? 1.0 : 0.0) * ClosestStarColor * max(0.0, 1.0 - adddata.a) * (1.0 - step(0.09, length(shipdata2.rgb))) * sunFlareColorizer * Exposure * 21.8 * snois;

    vec3 sunflare2 = flare(UV, projectedSunDir) * (1000.0 / length(ClosestStarPosition)) * (1.0 - step(0.01, adddata2.a)) * Exposure * (ClosestStarColor) * 0.14 * pow(max(0.0, -dot(dir, starDir)), 3.0);

    a += adddata.rgb + sunflare2;// + adddata2.rgb + sunflare2;
    //a = celestial.aaa * 1000.0;
    /*
    float shipdata3 = texture(texModelsDistance, UV).r;
    vec3 albedo = shipdata1.rgb;
    vec3 emission = texture(texModelsEmission, UV).rgb;
    float roughness = shipdata1.a;
    vec3 normal = normalize(shipdata2.rgb);
    float metalness = shipdata2.a;
    vec3 position = dir * shipdata3;
    vec3 positionCelestial = dir * adddata.a;
    vec3 viewdir = dir;
    vec3 lightdir = -starDir;
    vec3 lightcolor = ClosestStarColor * 0.0001;

    vec3 shadowModelsSpace = ((hiFreq.FromStarToThisMatrix) * vec4(position * 0.01, 1.0)).rgb;
    shadowModelsSpace.y *= -1.0;
    float readZ = texture(texModelsDistanceShadow, shadowModelsSpace.xy * 0.5 + 0.5).r;
    float target = (-shadowModelsSpace.z * 0.5 + 0.5) - 0.0001;
    float isShadow = smoothstep(-0.0001, 0.0001, target - readZ);


    vec3 shadowCeleSpace = ((hiFreq.FromStarToThisMatrix) * vec4(positionCelestial * 0.001, 1.0)).rgb;
    shadowCeleSpace.y *= -1.0;
    float readZ2 = texture(texModelsDistanceShadow, shadowCeleSpace.xy * 0.5 + 0.5).r;
    float target2 = (-shadowCeleSpace.z * 0.5 + 0.5) - 0.0001;
    float isShadow2 = smoothstep(-0.0001, 0.0001, target2 - readZ2);

    vec3 shaded = isShadow * vec3(1.0);//shade_ray(albedo, normal, viewdir, roughness, metalness, lightdir, lightcolor);
    shaded += albedo * 0.0 + emission;

    a = mix(a * isShadow2, shaded, step(0.09, length(shipdata2.rgb))) + sunflare2;*/
    //a += max(vec3(0.0), sunflare2);
    //vec4 particlesData = texture(texParticlesResult, UV).rgba;
//    a += particlesData.a == 0.0 ? vec3(0.0) : (particlesData.rgb);

    outColor = vec4(aces_tonemap(clamp(a, 0.0, 10000.0)), 1.0);

}
