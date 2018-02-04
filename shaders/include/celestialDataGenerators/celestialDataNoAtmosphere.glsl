#pragma once

float celestialNoAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    vec4 coord = vec4(dir, body.seed);
    return generateTerrain(coord);
}

vec4 celestialNoAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = body.sufraceMainColor;
    vec3 hsvColor = rgb2hsv(baseColor);
    float variance = height;
    vec3 newHsvColor = vec3(mix(hsvColor.x, 0.15, variance),
         mix(hsvColor.x, 0.99, variance),
         mix(hsvColor.x, 0.1, variance));
    vec3 newRgbColor = hsv2rgb(newHsvColor);
    return vec4(newRgbColor, 1.0);
}

vec2 celestialNoAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return vec2(0.0);
}
