#pragma once

#define CELESTIAL_RENDER_METHOD_NO_ATMOSPHERE 1
#define CELESTIAL_RENDER_METHOD_LIGHT_ATMOSPHERE 2
#define CELESTIAL_RENDER_METHOD_THICK_ATMOSPHERE 3

struct RenderedCelestialBody {
    int renderMethod;
    int samplerSet;
    vec3 position;
    float radius;
    Sphere surfaceSphere;
    Sphere atmosphereSphere;
    float seed;
    vec3 sufraceMainColor;
    float terrainMaxLevel;
    float fluidMaxLevel;
    float habitableChance; //render green according
    float atmosphereAbsorbStrength;
    vec3 atmosphereAbsorbColor;
};

struct CelestialBodyAlignedData {
    ivec4 renderMethod_samplerSet_zero_zero;
    vec4 position_radius;
    vec4 sufraceMainColor_atmosphereHeight;
    vec4 seed_terrainMaxLevel_fluidMaxLevel_habitableChance;
    vec4 sufraceMainColor_zero;
    vec4 atmosphereAbsorbColor_atmosphereAbsorbStrength;
};

RenderedCelestialBody getRenderedBody(CelestialBodyAlignedData aligned){
    return RenderedCelestialBody(
        aligned.renderMethod_samplerSet_zero_zero.x,
        aligned.renderMethod_samplerSet_zero_zero.y,
        aligned.position_radius.xyz,
        aligned.position_radius.a,
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a),
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a + aligned.sufraceMainColor_atmosphereHeight.a),
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.x, //seed
        aligned.sufraceMainColor_zero.xyz, // color
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.y, //terrainMaxLevel
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.z, //fluidMaxLevel
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.w, //habitableChance
        aligned.atmosphereAbsorbColor_atmosphereAbsorbStrength.w,
        aligned.atmosphereAbsorbColor_atmosphereAbsorbStrength.rgb
    );
}

struct RenderPass {
        Ray ray;
        RenderedCelestialBody body;
        float surfaceHit;
        float atmosphereNearHit;
        float atmosphereFarHit;
        vec3 surfaceHitPos;
        vec3 atmosphereNearHitPos;
        vec3 atmosphereFarHitPos;
        bool isAtmosphereHit;
        bool isSurfaceHit;
};

layout(set = 0, binding = 4) uniform sampler2D celestialHeightImage_set0;
layout(set = 0, binding = 5) uniform sampler2D celestialColorImage_set0;
layout(set = 0, binding = 6) uniform sampler2D celestialAtmosphereImage_set0;

layout(set = 0, binding = 7) uniform sampler2D celestialHeightImage_set1;
layout(set = 0, binding = 8) uniform sampler2D celestialColorImage_set1;
layout(set = 0, binding = 9) uniform sampler2D celestialAtmosphereImage_set1;

layout(set = 0, binding = 10) uniform sampler2D celestialHeightImage_set2;
layout(set = 0, binding = 11) uniform sampler2D celestialColorImage_set2;
layout(set = 0, binding = 12) uniform sampler2D celestialAtmosphereImage_set2;

layout(set = 0, binding = 13) uniform sampler2D celestialHeightImage_set3;
layout(set = 0, binding = 14) uniform sampler2D celestialColorImage_set3;
layout(set = 0, binding = 15) uniform sampler2D celestialAtmosphereImage_set3;

layout(set = 0, binding = 16) uniform sampler2D celestialHeightImage_set4;
layout(set = 0, binding = 17) uniform sampler2D celestialColorImage_set4;
layout(set = 0, binding = 18) uniform sampler2D celestialAtmosphereImage_set4;

struct CelestialSamplerSet {
    sampler2D heightImage;
    sampler2D colorImage;
    sampler2D atmosphereImage;
};

sampler2D getCelestialHeightSampler(RenderedCelestialBody body){
    switch(body.samplerSet){
        case 0: return celestialHeightImage_set0;
        case 1: return celestialHeightImage_set1;
        case 2: return celestialHeightImage_set2;
        case 3: return celestialHeightImage_set3;
        case 4: return celestialHeightImage_set4;
    }
}

sampler2D getCelestialColorSampler(RenderedCelestialBody body){
    switch(body.samplerSet){
        case 0: return celestialColorImage_set0;
        case 1: return celestialColorImage_set1;
        case 2: return celestialColorImage_set2;
        case 3: return celestialColorImage_set3;
        case 4: return celestialColorImage_set4;
    }
}

sampler2D getCelestialAtmosphereSampler(RenderedCelestialBody body){
    switch(body.samplerSet){
        case 0: return celestialAtmosphereImage_set0;
        case 1: return celestialAtmosphereImage_set1;
        case 2: return celestialAtmosphereImage_set2;
        case 3: return celestialAtmosphereImage_set3;
        case 4: return celestialAtmosphereImage_set4;
    }
}

#define DISTANCE_INFINITY 99999999.0

float raymarchCelestialTerrain(Ray ray, sampler2D s, RenderedCelestialBody body, float limit){
    float dist = 0.0;
    float maxheight2 = body.radius + body.terrainMaxLevel;
    float lastdst = DISTANCE_INFINITY;
    vec3 center = body.position;
    for(int i=0;i<700;i++){
        vec3 p = ray.o + ray.d * dist;
        vec3 dir = normalize(p - center);
        float dc = distance(p, center);
        float ds = dc - (body.radius + texture(s, xyzToPolar(dir)).r * body.terrainMaxLevel);
        if(ds < limit) return dist;
        if(dc > maxheight2 && lastdst < dc) return -0.01;
        lastdst = dc;
        dist += ds * 0.3;
    }
    return -0.01;
}

vec4 celestialGetColorRoughnessForDirection(RenderedCelestialBody body, vec3 direction){
    return texture(getCelestialColorSampler(body), xyzToPolar(direction)).rgba;
}
vec4 celestialGetColorRoughnessRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetColorRoughnessForDirection(body, normalize(position - body.position));
}

void updatePassHits(inout RenderPass pass){
    float hit_Surface = rsi2(pass.ray, pass.body.surfaceSphere).y;
    if(distance(pass.body.position, pass.ray.o) < pass.body.radius * 4.0){
        hit_Surface = raymarchCelestialTerrain(pass.ray, getCelestialHeightSampler(pass.body), pass.body, 1.0);
    }
    vec2 hits_Atmosphere = rsi2(pass.ray, pass.body.atmosphereSphere);
    if(hit_Surface > 0.0 && hit_Surface < DISTANCE_INFINITY) {
        pass.isSurfaceHit = true;
        pass.surfaceHit = hit_Surface;
        pass.surfaceHitPos = pass.ray.o + pass.ray.d * pass.surfaceHit;
    }
    if(hits_Atmosphere.y > 0.0 && hits_Atmosphere.y < DISTANCE_INFINITY) {
        pass.isAtmosphereHit = true;
        pass.atmosphereNearHit = hits_Atmosphere.x;
        pass.atmosphereFarHit = hits_Atmosphere.y;
        pass.atmosphereNearHitPos = pass.ray.o + pass.ray.d * pass.atmosphereNearHit;
        pass.atmosphereFarHitPos = pass.ray.o + pass.ray.d * pass.atmosphereFarHit;
    }
}

#include celestialNoAtmosphere.glsl
#include celestialLightAtmosphere.glsl
#include celestialThickAtmosphere.glsl

vec4 renderCelestialBody(RenderedCelestialBody body, Ray ray){
    RenderPass pass = RenderPass(ray, body, 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), false, false);
    updatePassHits(pass);
    vec4 result = vec4(0.0);
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_NO_ATMOSPHERE){
        if(pass.isSurfaceHit){
            result = renderCelestialBodyNoAtmosphere(pass);
        }
    }
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_LIGHT_ATMOSPHERE){
        if(pass.isAtmosphereHit){
            result = renderCelestialBodyLightAtmosphere(pass);
        }
    }
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_THICK_ATMOSPHERE){
        if(pass.isAtmosphereHit){
            result = renderCelestialBodyThickAtmosphere(pass);
        }
    }
    return vec4(0.0);
}
