#pragma once

float celestialLightAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    vec4 coord = vec4(dir, body.seed);
    return pow(generateTerrain(coord), 9.0);
}

vec4 celestialLightAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = body.sufraceMainColor;
    float polardir = abs(dot(dir, vec3(0.0, 1.0, 0.0)));
    vec3 dominativeHot = baseColor * vec3(1.0, 0.7, 0.7);
    vec3 dominativeCold = mix(baseColor * vec3(0.5, 0.5, 1.0), vec3(1.0), 0.5);
    vec3 polarawarecolor = mix(dominativeHot, dominativeCold, pow(polardir, 3.0));
    dir *= 0.4 + 3.0 * oct(body.seed);
    vec3 colorrandomizer = vec3(
        getwaves(dir * 1.0, 10.0, 0.0, body.seed),
        getwaves(dir * 1.0, 10.0, 0.0, body.seed + 222.0 ),
        getwaves(dir * 1.0, 10.0, 0.0, body.seed + 422.0)
    );
    float dimmer = 0.2 + (1.0 - getwaves(dir * 1.0, 10.0, 0.0, body.seed + 422.0) * 0.8);

    vec3 groundColor = polarawarecolor * dimmer - colorrandomizer * (1.0 - height);
    return vec4(groundColor, 1.0);
}

#include cloudsRendering.glsl

vec2 celestialLightAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return getLowAndHighClouds(dir, body.seed);
}
