#pragma once

float celestialNoAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    vec4 coord = vec4(dir, body.seed);
    return generateTerrain(coord);
}

vec4 celestialNoAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = body.sufraceMainColor;
    float polardir = abs(dot(dir, vec3(0.0, 1.0, 0.0)));
    vec3 dominativeHot = baseColor * vec3(1.0, 0.7, 0.7);
    vec3 dominativeCold = mix(baseColor * vec3(0.5, 0.5, 1.0), vec3(1.0), 0.5);
    vec3 polarawarecolor = mix(dominativeHot, dominativeCold, pow(polardir, 3.0));
    vec3 colorrandomizer = vec3(
        wavesOctaveNoise(dir * 10.0),
        wavesOctaveNoise(-dir * 10.0),
        wavesOctaveNoise(dir * 10.0+100.0)
    );

    vec3 groundColor = polarawarecolor - colorrandomizer * (1.0 - height);
    return vec4(groundColor, 1.0);
}

vec2 celestialNoAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return vec2(0.0);
}
