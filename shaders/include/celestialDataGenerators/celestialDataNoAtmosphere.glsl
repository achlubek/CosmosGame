#pragma once

float craterSidesFallof(float central, float val, float slopea, float slopeb){
    return pow(smoothstep(central- slopea, central, val) * (1.0 - smoothstep(central, central + slopeb, val)), 3.0);
}

float crater(vec3 dir, float seed){
    vec3 center = normalize(vec3(oct(seed), oct(seed + 1.0), oct(seed + 2.0)) * 2.0 - 1.0);
    float scaling = oct(seed + 3.0);
    float radius = scaling * 0.2 + 0.002;
    float dist = distance(dir, center);
    dist *= noise4d(vec4(dir, seed) * 20.0)*0.2 + 1.0;
    float c1 = craterSidesFallof(radius, dist, radius, radius);//1.0 / (1.0 + abs(dist - radius) * 3.0 / (scaling * 0.3 + 0.1 ));
    float c2 = 1.0 - smoothstep(0.0, radius * 0.3, dist);
    return c1 + c2 * 0.1;
}

float celestialNoAtmosphereGetHeightMap(RenderedCelestialBody body, vec3 dir){
    vec4 coord = vec4(dir, sin(body.seed));
    float h = generateTerrain(coord);
    float h2 = h;
    float seed = body.seed;
    for(int i=0;i<int(oct(body.seed) * 1000.0);i++){
        h += h2 * crater(dir, oct(seed += 7.0));
    }
    return h;
}

vec4 celestialNoAtmosphereGetColorRoughnessMap(RenderedCelestialBody body, float height, vec3 dir){
    if(oct(body.seed) > 0.5){
        vec3 baseColor = normalize(body.sufraceMainColor);
        dir *= 0.4 + 3.0 * oct(sin(body.seed));
        vec3 colorrandomizer = vec3(oct(sin(body.seed)), oct(sin(body.seed) + 6), oct(sin(body.seed) + 12.0)) *
        (FBM3(dir * 1.0 + sin(body.seed), 6, 2.0, 0.66)* 0.5
        + getwaves(dir * 2.0 * noise4d(vec4(dir, sin(body.seed))), 15.0, 0.0, sin(body.seed))* 0.25 + noise4d(vec4(dir, sin(body.seed))) * 0.125);
        float dimmer = getwaves(dir * 1.0, 10.0, 0.0, sin(body.seed) + 422.0);
        vec3 groundColor = mix((colorrandomizer + baseColor) * 0.5, baseColor, sin(height + dimmer * 10.0) * 0.5 + 0.5);
        groundColor = mix(groundColor, vec3(length(groundColor)), 0.5);
        return vec4(groundColor + vec3(1.0 / 256.0) * oct(dir), 1.0);
    } else {
        vec3 baseColor = normalize(body.sufraceMainColor);
        dir *= 0.4 + 3.0 * oct(sin(body.seed ));
        vec3 colorrandomizer = baseColor * 0.5 + 0.5 * vec3(
            FBM4(vec4(dir * 20.0, body.seed), 10, 2.0, 0.5),
            FBM4(vec4(dir * 24.0, body.seed + 112.0), 10, 2.0, 0.5),
            FBM4(vec4(dir * 27.0, body.seed - 121.0), 10, 2.0, 0.5)
        );

        vec3 groundColor = pow(clamp(mix(colorrandomizer, baseColor, clamp(FBM4(vec4(dir * 10.0, body.seed), 3, 2.0, 0.5), 0.0, 1.0)), 0.0, 1.0), vec3(4.0));
        return vec4(colorrandomizer + vec3(1.0 / 256.0) * oct(dir), 1.0);
    }
}

vec2 celestialNoAtmosphereGetCloudsMap(RenderedCelestialBody body, float height, vec3 dir){
    return vec2(0.0);
}
