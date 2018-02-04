#pragma once

float celestialThickAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    return 0.0;
}

vec4 celestialThickAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    vec3 baseColor = body.sufraceMainColor;
    vec3 hsvColor = rgb2hsv(baseColor);
    float variance = FBM3(dir * 10.0 * vec3(1.0, 10.0, 1.0) + body.seed * 10.0, 4, 2.0, 0.5);
    vec3 newHsvColor = vec3(fract(hsvColor.x + variance * 0.5), hsvColor.y, hsvColor.z);
    vec3 newRgbColor = hsv2rgb(newHsvColor);
    return vec4(newRgbColor, 1.0);
}

vec2 celestialThickAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return vec2(0.0);
}
