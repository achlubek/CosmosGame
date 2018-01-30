#pragma once

#define CELESTIAL_RENDER_METHOD_NO_ATMOSPHERE 1
#define CELESTIAL_RENDER_METHOD_LIGHT_ATMOSPHERE 2
#define CELESTIAL_RENDER_METHOD_THICK_ATMOSPHERE 3

struct RenderedCelestialBody {
    int renderMethod;
    vec3 position;
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
    ivec4 renderMethod;
    vec4 position_radius;
    vec4 sufraceMainColor_atmosphereHeight;
    vec4 seed_terrainMaxLevel_fluidMaxLevel_habitableChance;
    vec4 sufraceMainColor_zero;
    vec4 atmosphereAbsorbColor_atmosphereAbsorbStrength;
};

RenderedCelestialBody getRenderedBody(CelestialBodyAlignedData aligned){
    return RenderedCelestialBody(
        aligned.renderMethod.x,
        aligned.position_radius.xyz,
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
        bool isAtmosphereHit;
        bool isSurfaceHit;
};

#define DISTANCE_INFINITY 99999999.0
void updatePassHits(inout RenderPass pass){
    vec2 hit_Surface = rsi2(pass.ray, pass.body.surfaceSphere);
    vec2 hits_Atmosphere = rsi2(pass.ray, pass.body.atmosphereSphere);
    if(hit_Surface.y > 0.0 && hit_Surface.y < DISTANCE_INFINITY) {
        pass.isSurfaceHit = true;
        pass.surfaceHit = hit_Surface.y;
    }
    if(hits_Atmosphere.y > 0.0 && hits_Atmosphere.y < DISTANCE_INFINITY) {
        pass.isAtmosphereHit = true;
        pass.atmosphereNearHit = hits_Atmosphere.x;
        pass.atmosphereFarHit = hits_Atmosphere.y;
    }
}

#include celestialNoAtmosphere.glsl
#include celestialLightAtmosphere.glsl
#include celestialThickAtmosphere.glsl

vec4 renderCelestialBody(RenderedCelestialBody body, Ray ray){
    RenderPass pass = RenderPass(ray, body, 0.0, 0.0, 0.0, false, false);
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
