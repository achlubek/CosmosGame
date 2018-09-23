#pragma once


float generateTerrain(vec4 coord){
float row = getwavesHighPhase3(coord.xyz * 1.0, 48, 0.2, coord.a, coord.a);
    coord.xyz *= 15.0 + oct(coord.a) * 19.0;
    float scaler = pow(getwaves(coord.xyz * 0.1, 14.0, 0.0, coord.a), 3.0) * 0.2 + 0.4;
    coord.xyz *= scaler * 1.0;
    float displacer = getwaves(coord.xyz, 7.0, 0.0, coord.a);
    vec3 displacer2 = vec3(aBitBetterNoise(coord), aBitBetterNoise(-coord.yxzw), aBitBetterNoise(-coord));
    float a = getwaves(coord.xyz + displacer * 0.4 + displacer2, 2.0 + oct(coord.a) * 13.0, 0.0, coord.a) * aBitBetterNoise(coord);
    a *= FBM4(coord * 2.0, 8, 2.0, 0.50) * 2.0;
    a = clamp(a * 1.7, 0.0, 1.0);
    float row2 = clamp(pow((1.0 - row) * 1.7, 4.0), 0.0, 1.0);
    return row2 * sqrt(a);//mix(1.0 - a * a, a * a, smoothstep(0.3, 0.6, noise4d(vec4(coord))));
}
