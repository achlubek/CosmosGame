#pragma once

float celestialLightAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    vec4 coord = vec4(dir, body.seed);
    return generateTerrain(coord);
}

vec4 celestialLightAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = body.sufraceMainColor;
    vec3 hsvColor = rgb2hsv(baseColor);
    float variance = height;
    vec3 newHsvColor = vec3(mix(hsvColor.x, 0.15, variance), hsvColor.y, hsvColor.z);
    vec3 groundColor = hsv2rgb(newHsvColor);
    vec3 waterColor = vec3(0.0, 0.3, 0.7);
    vec3 resultColor = mix(waterColor, groundColor, smoothstep(body.fluidMaxLevel, body.fluidMaxLevel*1.004, height * 2.0 * body.terrainMaxLevel));

    return vec4(resultColor, 1.0);
}
vec2 celestialLightAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    float cloudsHi = FBM3(dir - body.seed, 4, 3.0, 0.6);
    float cloudsLow = FBM3(dir * 0.2 +-body.seed, 4, 3.0, 0.6);
    cloudsHi = smoothstep(0.2, 0.6, cloudsHi);
    cloudsLow = smoothstep(0.5, 0.6, cloudsLow);
    return vec2(cloudsLow, cloudsHi);
}
