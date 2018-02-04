#pragma once

#define CELESTIAL_RENDER_METHOD_NO_ATMOSPHERE 1
#define CELESTIAL_RENDER_METHOD_LIGHT_ATMOSPHERE 2
#define CELESTIAL_RENDER_METHOD_THICK_ATMOSPHERE 3

struct RenderedCelestialBody {
    int renderMethod;
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
    ivec4 renderMethod_zero_zero_zero;
    vec4 position_radius;
    vec4 sufraceMainColor_atmosphereHeight;
    vec4 seed_terrainMaxLevel_fluidMaxLevel_habitableChance;
    vec4 atmosphereAbsorbColor_atmosphereAbsorbStrength;
};

RenderedCelestialBody getRenderedBody(CelestialBodyAlignedData aligned){
    return RenderedCelestialBody(
        aligned.renderMethod_zero_zero_zero.x,
        aligned.position_radius.xyz,
        aligned.position_radius.a,
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a),
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a + aligned.sufraceMainColor_atmosphereHeight.a),
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.x, //seed
        aligned.sufraceMainColor_atmosphereHeight.xyz, // color
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.y, //terrainMaxLevel
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.z, //fluidMaxLevel
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.w, //habitableChance
        aligned.atmosphereAbsorbColor_atmosphereAbsorbStrength.w,
        aligned.atmosphereAbsorbColor_atmosphereAbsorbStrength.rgb
    );
}