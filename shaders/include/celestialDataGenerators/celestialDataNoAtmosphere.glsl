#pragma once

float celestialNoAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    vec4 coord = vec4(dir, sin(body.seed));
    return generateTerrain(coord);
}

vec4 celestialNoAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = body.sufraceMainColor;
    float polardir = abs(dot(dir, vec3(0.0, 1.0, 0.0)));
    dir *= 0.4 + 3.0 * oct(sin(body.seed));
    vec3 colorrandomizer = vec3(oct(sin(body.seed)), oct(sin(body.seed) + 6), oct(sin(body.seed) + 12.0)) *
    (FBM3(dir * 1.0 + sin(body.seed), 6, 2.0, 0.66)* 0.5
    + getwaves(dir * 2.0 * noise4d(vec4(dir, sin(body.seed))), 15.0, 0.0, sin(body.seed))* 0.25 + noise4d(vec4(dir, sin(body.seed))) * 0.125);
    float dimmer = getwaves(dir * 1.0, 10.0, 0.0, sin(body.seed) + 422.0);
    vec3 groundColor = mix((colorrandomizer + baseColor) * 0.5, baseColor, sin(height + dimmer * 10.0) * 0.5 + 0.5);
    groundColor = mix(groundColor, vec3(length(groundColor)), 0.5);
    return vec4(groundColor + vec3(1.0 / 256.0) * oct(dir), 1.0);
}

vec2 celestialNoAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return vec2(0.0);
}
