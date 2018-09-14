#pragma once

float celestialThickAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    return 0.0;
}

vec4 celestialThickAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = body.sufraceMainColor;

    float windR = FBM3(dir * 3.0 * vec3(1.0, 4.0, 1.0) + sin(body.seed * 1.0) * 10.0, 6, 2.0, 0.6);
    float windG = FBM3(dir * 3.0 * vec3(1.0, 4.0, 1.0) + sin(body.seed * 1.0) * 50.0, 6, 2.0, 0.6);
    float windB = FBM3(dir * 3.0 * vec3(1.0, 4.0, 1.0) + sin(body.seed * 1.0) * 660.0, 6, 2.0, 0.6);

    dir += (vec3(windR, windG, windB) * 2.0 - 1.0) * 0.1;

    float varianceSaturation = FBM3(dir * 4.0 * vec3(1.0, 7.0, 1.0) + sin(body.seed * 1.0) * 10.0, 6, 2.2, 0.66);
    float varianceR = FBM3(dir * 6.0 * vec3(1.0, 7.0, 1.0) + sin(body.seed * 1.0) * 10.0, 4, 2.0, 0.5);
    float varianceG = FBM3(dir * 6.0 * vec3(1.0, 7.0, 1.0) + sin(body.seed * 1.0) * 50.0, 4, 2.0, 0.5);
    float varianceB = FBM3(dir * 6.0 * vec3(1.0, 7.0, 1.0) + sin(body.seed * 1.0) * 660.0 , 4, 2.0, 0.5);

    vec3 newRgbColor = mix(baseColor, vec3(length(baseColor)), varianceSaturation) * varianceSaturation * (vec3(varianceR, varianceG, varianceB) * 0.5 + 0.5);
    return vec4(newRgbColor, 1.0);
}

vec2 celestialThickAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return vec2(0.0);
}
