#pragma once


float generateTerrain(vec4 coord){
    coord.xyz *= 5.0 + oct(coord.a) * 19.0;
    float scaler = pow(getwaves(coord.xyz * 0.1, 14.0, 0.0, coord.a), 3.0) * 0.2 + 0.4;
    coord.xyz *= scaler * 1.0;
    float displacer = getwaves(coord.xyz, 7.0, 0.0, coord.a);
    vec3 displacer2 = vec3(aBitBetterNoise(coord), aBitBetterNoise(-coord.yxzw), aBitBetterNoise(-coord));
    float a = getwaves(coord.xyz + displacer * 0.4 + displacer2, 2.0 + oct(coord.a) * 13.0, 0.0, coord.a) * aBitBetterNoise(coord);
    a = clamp(a * 1.7, 0.0, 1.0);
    return mix(1.0 - a * a, a * a, smoothstep(0.3, 0.6, noise4d(vec4(coord))));
}
