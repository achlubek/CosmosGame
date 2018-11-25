float fbmclouds( vec4 p )
{
    float f = 0.0;
    f += 0.50000*aBitBetterNoise( p ); p = p*2.02;
    f -= 0.25000*aBitBetterNoise( p ); p = p*2.03;
    f += 0.12500*aBitBetterNoise( p ); p = p*3.01;
    f += 0.06250*aBitBetterNoise( p ); p = p*3.04;
    f += 0.03500*aBitBetterNoise( p ); p = p*4.01;
    f += 0.01250*aBitBetterNoise( p ); p = p*4.04;
    f -= 0.00125*aBitBetterNoise( p );
    return f/0.984375;
}

float cloud(vec4 p)
{
    p-=fbmclouds(vec4(p.x,p.y, p.z,0.0)*0.5)*1.25;
    float a = min((fbmclouds(p*3.0)*2.2-1.1), 0.0);
    return a*a;
}

#define HOW_CLOUDY 0.4
#define SHADOW_THRESHOLD 0.2
#define SHADOW 0.2
#define SUBSURFACE 1.0
#define WIND_DIRECTION 2.0
#define TIME_SCALE 0.0
#define SCALE 0.5
#define Time 0.0 //Stub

float clouds(vec3 p, float seed){
    float howCloudy = oct(seed + 1.0) * 0.6;
    float scaling = oct(seed + 3.0);
    float shift = oct(seed + 5.0);
    p *= 0.5 + scaling;
    float ic = cloud(vec4(p * 2.0, shift)) / howCloudy;
    float init = smoothstep(0.1, 1.0, ic) * 10.0;
    init = (init * cloud(vec4(p * (6.0), shift)) * ic);
    init = (init * (cloud(vec4(p * (11.0), shift))*0.5 + 0.4) * init);
    return min(1.0, init);
}

vec2 getLowAndHighClouds(vec3 dir, float seed){
    dir *= 0.5;
    dir *= 0.5 + 2.0 * oct(sin(seed + 12.01223));

    vec3 wind = vec3(
        getwaves(dir * 1.0, 5.0, 0.0, 0.0),
        getwaves(dir * 1.0, 5.0, 0.0, 100.0),
        getwaves(dir * 1.0, 5.0, 0.0, 200.0)
        ) * 2.0 - 1.0;
    float scale = 0.3 + 1.0 * aBitBetterNoise(vec4(dir * 3.2 - 10.0, 1.0));
    float experimental = getwaves(scale * (dir * 10.0 + wind * 5.0), 13.0, 13.0, 3.0) * getwaves(scale * (dir * 3.0 + wind * 5.0), 13.0, 13.0, 3.0)
     + getwaves(scale * (dir * 40.0 + wind * 5.0), 13.0, 13.0, 3.0) * 0.1;
    float threshold = 0.0015 + 0.2 * aBitBetterNoise(vec4(dir * 3.2 , 1.0)) + 0.3 * oct(sin(seed));
    float range = 0.3* aBitBetterNoise(vec4(dir * 3.2 + 10.0 , 1.0)) + 0.3 * oct(sin(seed + 12.0));
    vec2 thickClouds = vec2(smoothstep(threshold, threshold + range, experimental), experimental);

    float rare = FBM4(vec4(dir, seed), 8, 2.0, 6.0);
    rare = FBM4(vec4(dir, seed) + rare, 8, 2.0, 6.0);
    return vec2(rare, 0.0);
}
