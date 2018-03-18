#pragma once

float craterSidesFallof(float central, float val, float slopea, float slopeb){
    return pow(smoothstep(central- slopea, central, val) * (1.0 - smoothstep(central, central + slopeb, val)), 3.0);
}
float crater(vec3 dir, float seed){
    vec3 center = normalize(vec3(oct(seed), oct(seed + 1.0), oct(seed + 2.0)) * 2.0 - 1.0);
    float scaling = oct(seed + 3.0)  * oct(seed + 4.0) * 3.0 + 0.1;
    float radius = scaling * 0.1 + 0.01;
    float dist = distance(dir, center);
    dist *= noise4d(vec4(dir, seed) * 20.0)*0.2 + 1.0;
    float c1 = craterSidesFallof(radius, dist, radius, radius);//1.0 / (1.0 + abs(dist - radius) * 3.0 / (scaling * 0.3 + 0.1 ));
    float c2 = 1.0 - smoothstep(0.0, radius * 0.1, dist);
    return c1 + c2;
}

float generateTerrainX(vec4 coord){
    float low_freq_plates = FBM4(coord * 5.0, 3, 1.5, 0.5);
    float mid_freq_lands = FBM4(coord * 13.0 + 4.0 * FBM4(coord * 3.0, 7, 2.0, 0.5), 7, 2.0, 0.5);
    float hi_freq_lands = FBM4(coord * 46.0 + 4.0 * FBM4(coord * 16.0, 7, 2.0, 0.5), 7, 2.0, 0.5);
    float craters2 = 0.0;
    float seed = coord.a;
    int craterscount = int(oct(coord.a)*100.0);
    float cratersStrength = 0.2 * oct(coord.a + 10.0);
    for(int i=0;i<craterscount;i++){
        craters2 += crater(coord.xyz, seed);
        seed += 10.0;
    }
    return pow(low_freq_plates + (mid_freq_lands * 0.25) + (hi_freq_lands * 0.1) + craters2 * cratersStrength, 3.0);
}

float generateTerrain(vec4 coord){
    coord.xyz *= 9.0 + oct(coord.a) * 6.0;
    float scaler = pow(getwaves(coord.xyz * 0.1, 14.0, 0.0, coord.a), 3.0) * 0.2 + 0.4;
    coord.xyz *= scaler * 1.0;
    float displacer = getwaves(coord.xyz, 7.0, 0.0, coord.a);
    vec3 displacer2 = vec3(aBitBetterNoise(coord), aBitBetterNoise(-coord.yxzw), aBitBetterNoise(-coord));
    float a = getwaves(coord.xyz + displacer * 0.4 + displacer2, 7.0, 0.0, coord.a) * aBitBetterNoise(coord);
    a = clamp(a * 1.7, 0.0, 1.0);
    return a * a * a;//clamp(pow(a * 1.0, 1.0) , 0.0, 1.0);//a * 3.6;//clamp((pow(a*0.0,2.0) + aBitBetterNoise(coord)) * 0.0, 0.0, 1.0);
}
