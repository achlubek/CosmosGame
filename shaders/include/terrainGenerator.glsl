float generateTerrain(vec4 coord){
    float low_freq_plates = pow(smoothstep(0.3, 0.6, FBM4(coord, 7, 1.5, 0.5)), 1.0);
    float mid_freq_lands = FBM4(coord * 2.0 + 4.0 * FBM4(coord * 10.0, 7, 1.5, 0.5), 7, 1.5, 0.5);
    float scaling = 0.2 + FBM4(coord * 2.0, 7, 1.5, 0.5) * 2.0;
    float craters = (1.0 - 2.0 * abs(0.5 - FBM4(coord * 5.0 * scaling + 0.3 * FBM4(coord * 5.0 * scaling, 7, 2.2, 0.5), 7, 2.2, 0.5)));
    return low_freq_plates * mid_freq_lands * craters;
}
