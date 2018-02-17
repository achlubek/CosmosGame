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

float clouds(vec3 p){
    float ic = cloud(vec4(p * 2.0, Time*0.01 * TIME_SCALE)) / HOW_CLOUDY;
    float init = smoothstep(0.1, 1.0, ic) * 10.0;
    init = (init * cloud(vec4(p * (6.0), Time*0.01 * TIME_SCALE)) * ic);
    init = (init * (cloud(vec4(p * (11.0), Time*0.01 * TIME_SCALE))*0.5 + 0.4) * init);
    return min(1.0, init);
}

#define EULER 2.7182818284590452353602874
float wave(vec3 uv, vec3 emitter, float speed, float phase, float timeshift){
    float dst = distance(uv, emitter);
    return pow(EULER, sin(dst * phase - (Time * TIME_SCALE + timeshift) * speed)) / EULER;
}
vec3 wavedrag(vec3 uv, vec3 emitter){
    return normalize(uv - emitter);
}

#define DRAG_MULT 4.0

float seed = 0.0;


vec3 rand3d(){
    float x = oct(seed);
    seed += 1.0;
    float y = oct(seed);
    seed += 1.0;
    float z = oct(seed);
    seed += 1.0;
    return vec3(x,y,z) * 2.0 - 1.0;
}

float getwaves(vec3 position){
    float iter = 0.0;
    float phase = 6.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<20;i++){
        vec3 p = rand3d() * 30.0;
        float res = wave(position, p, speed, phase, 0.0);
        float res2 = wave(position, p, speed, phase, 0.006);
        position -= wavedrag(position, p) * (res - res2) * weight * DRAG_MULT;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.02;
        speed *= 1.02;
    }
    return w / ws;
}

vec2 getLowAndHighClouds(vec3 dir){
    float low = 1.0 - clouds(dir);
    float c = cloud(vec4(dir, 1.0));
    float high = smoothstep(0.3, 0.6, c) * getwaves(mix(0.5, 1.5, c) * dir * 40.0) * 2.125;
    return vec2(low, high);
}
