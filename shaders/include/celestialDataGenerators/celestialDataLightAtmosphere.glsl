#pragma once

float celestialLightAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    vec4 coord = vec4(dir, sin(body.seed ));
    return generateTerrain(coord) * 0.80;
}

vec4 celestialLightAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = body.sufraceMainColor;
    float polardir = abs(dot(dir, vec3(0.0, 1.0, 0.0)));
    dir *= 0.4 + 3.0 * oct(sin(body.seed ));
    vec3 colorrandomizer = (1.0 - body.sufraceMainColor) * (getwaves(dir * 1.0, 5.0, 0.0, sin(body.seed ))* 0.5
        + getwaves(dir * 2.0 * noise4d(vec4(dir, sin(body.seed ))), 15.0, 0.0, sin(body.seed ))* 0.25 + noise4d(vec4(dir, sin(body.seed ))) * 0.125);
    float dimmer = getwaves(dir * 1.0, 10.0, 0.0, sin(body.seed ) + 422.0);

    vec3 groundColor = mix(colorrandomizer, baseColor, sin(height + dimmer * 10.0) * 0.5 + 0.5);
    return vec4(groundColor + vec3(1.0 / 256.0) * oct(dir), 1.0);
}

#include cloudsRendering.glsl

vec2 celestialLightAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return getLowAndHighClouds(dir, sin(body.seed ));
}
