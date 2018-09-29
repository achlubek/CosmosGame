#pragma once

#define CELESTIAL_RENDER_METHOD_NO_ATMOSPHERE 1
#define CELESTIAL_RENDER_METHOD_LIGHT_ATMOSPHERE 2
#define CELESTIAL_RENDER_METHOD_THICK_ATMOSPHERE 3

struct RenderedCelestialBody {
    int renderMethod;
    vec3 position;
    float radius;
    float atmosphereRadius;
    float atmosphereHeight;
    Sphere surfaceSphere;
    Sphere surfaceLowSphere;
    Sphere waterSphere;
    Sphere atmosphereSphere;
    Sphere highCloudsSphere;
    Sphere lowCloudsTopSphere;
    Sphere lowCloudsBottomSphere;
    float seed;
    vec3 sufraceMainColor;
    float terrainMaxLevel;
    float fluidMaxLevel;
    float habitableChance; //render green according
    float atmosphereAbsorbStrength;
    vec3 atmosphereAbsorbColor;
    mat3 rotationMatrix;
    mat3 fromHostToThisMatrix;
};

struct CelestialBodyAlignedData {
    ivec4 renderMethod_zero_zero_zero;
    vec4 position_radius;
    vec4 sufraceMainColor_atmosphereHeight;
    vec4 seed_terrainMaxLevel_fluidMaxLevel_habitableChance;
    vec4 atmosphereAbsorbColor_atmosphereAbsorbStrength;
    mat4 rotationMatrix;
    mat4 fromHostToThisMatrix;
};

RenderedCelestialBody getRenderedBody(CelestialBodyAlignedData aligned){
    return RenderedCelestialBody(
        aligned.renderMethod_zero_zero_zero.x,
        aligned.position_radius.xyz,
        aligned.position_radius.a ,
        aligned.position_radius.a + aligned.sufraceMainColor_atmosphereHeight.a,
        aligned.sufraceMainColor_atmosphereHeight.a,
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a + aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.y),
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a),
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a + aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.z),
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a + aligned.sufraceMainColor_atmosphereHeight.a),
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a + aligned.sufraceMainColor_atmosphereHeight.a * 0.75),
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a + aligned.sufraceMainColor_atmosphereHeight.a * 0.25),
        Sphere(aligned.position_radius.xyz, aligned.position_radius.a + aligned.sufraceMainColor_atmosphereHeight.a * 0.20),
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.x, //seed
        aligned.sufraceMainColor_atmosphereHeight.xyz, // color
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.y, //terrainMaxLevel
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.z, //fluidMaxLevel
        aligned.seed_terrainMaxLevel_fluidMaxLevel_habitableChance.w, //habitableChance
        aligned.atmosphereAbsorbColor_atmosphereAbsorbStrength.w,
        aligned.atmosphereAbsorbColor_atmosphereAbsorbStrength.rgb,
        mat3(aligned.rotationMatrix),
        mat3(aligned.fromHostToThisMatrix)
    );
}
