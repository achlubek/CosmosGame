#pragma once

float celestialLightAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    vec4 coord = vec4(dir, sin(body.seed ));
    return generateTerrain(coord) * 0.80;
}

vec4 celestialLightAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = normalize(body.sufraceMainColor);
    dir *= 0.4 + 3.0 * oct(sin(body.seed ));
    vec3 colorrandomizer = baseColor * 0.5 + 0.5 * vec3(
        FBM4(vec4(dir * 20.0, body.seed), 10, 2.0, 0.5),
        FBM4(vec4(dir * 24.0, body.seed + 112.0), 10, 2.0, 0.5),
        FBM4(vec4(dir * 27.0, body.seed - 121.0), 10, 2.0, 0.5)
    );
    float dimmer = getwavesHighPhase3(dir * 1.0, 48, 0.2, body.seed, body.seed);
    colorrandomizer *= 0.3 + 0.7 * clamp(pow((1.0 - dimmer) * 2.0, 4.0), 0.0, 1.0);

    vec3 groundColor = pow(clamp(mix(colorrandomizer, baseColor, clamp(FBM4(vec4(dir * 10.0, body.seed), 3, 2.0, 0.5), 0.0, 1.0)), 0.0, 1.0), vec3(4.0));
    return vec4(colorrandomizer + vec3(1.0 / 256.0) * oct(dir), 1.0);
}

#include cloudsRendering.glsl

vec2 celestialLightAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return getLowAndHighClouds(dir, sin(body.seed ));
}
