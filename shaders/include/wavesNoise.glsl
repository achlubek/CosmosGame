#pragma once
/*
MIT License

Copyright (c) 2018 Adrian Chlubek

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include hashers.glsl

#define EULER 2.7182818284590452353602874
// all variations from 1d to 4d
float wave(float uv, float emitter, float speed, float phase, float timeshift){
    return pow(EULER, sin(abs(uv - emitter) * phase - timeshift * speed) - 1.0);
}
float wave(vec2 uv, vec2 emitter, float speed, float phase, float timeshift){
    return pow(EULER, sin(distance(uv, emitter) * phase - timeshift * speed) - 1.0);
}
float wave(vec3 uv, vec3 emitter, float speed, float phase, float timeshift){
    return pow(EULER, sin(distance(uv, emitter) * phase - timeshift * speed) - 1.0);
}
float wave(vec4 uv, vec4 emitter, float speed, float phase, float timeshift){
    return pow(EULER, sin(distance(uv, emitter) * phase - timeshift * speed) - 1.0);
}

vec2 wavedx(vec3 uv, vec3 emitter, float speed, float phase, float timeshift){
    float x = distance(uv, emitter) * phase - timeshift * speed;
    float wave = exp(sin(x) - 1.0);
    float dx = wave * cos(x);
    return vec2(wave, dx);
}

float getwaves(float position, float dragmult, float timeshift, float seed){
    float iter = 0.0;
    float seedWaves = seed;
    float phase = 6.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<35;i++){
        float p = (oct(seedWaves += 1.0) * 2.0 - 1.0) * 300.0;
        float res = wave(position, p, speed, phase, 0.0 + timeshift);
        float res2 = wave(position, p, speed, phase, 0.006 + timeshift);
        position -= normalize(position - p) * (res - res2) * weight * dragmult;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
        speed *= 1.02;
    }
    return w / ws;
}

float getwaves(vec2 position, float dragmult, float timeshift, float seed){
    float iter = 0.0;
    float seedWaves = seed;
    float phase = 6.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<35;i++){
        vec2 p = (vec2(oct(seedWaves += 1.0), oct(seedWaves += 1.0)) * 2.0 - 1.0) * 300.0;
        float res = wave(position, p, speed, phase, 0.0 + timeshift);
        float res2 = wave(position, p, speed, phase, 0.006 + timeshift);
        position -= normalize(position - p) * (res - res2) * weight * dragmult;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
        speed *= 1.02;
    }
    return w / ws;
}

float getwaves(vec3 position, float dragmult, float timeshift, float seed){
    float iter = 0.0;
    float seedWaves = seed;
    float phase = 6.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<35;i++){
        vec3 p = (vec3(oct(seedWaves += 1.0), oct(seedWaves += 1.0), oct(seedWaves += 1.0)) * 2.0 - 1.0) * 300.0;
        float res = wave(position, p, speed, phase, 0.0 + timeshift);
        float res2 = wave(position, p, speed, phase, 0.006 + timeshift);
        position -= normalize(position - p) * (res - res2) * weight * dragmult;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
        speed *= 1.02;
    }
    return w / ws;
}

float getwaves(vec4 position, float dragmult, float timeshift, float seed){
    float iter = 0.0;
    float seedWaves = seed;
    float phase = 6.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<35;i++){
        vec4 p = (vec4(oct(seedWaves += 1.0), oct(seedWaves += 1.0), oct(seedWaves += 1.0), oct(seedWaves += 1.0)) * 2.0 - 1.0) * 300.0;
        float res = wave(position, p, speed, phase, 0.0 + timeshift);
        float res2 = wave(position, p, speed, phase, 0.006 + timeshift);
        position -= normalize(position - p) * (res - res2) * weight * dragmult;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.2;
        speed *= 1.02;
    }
    return w / ws;
}


vec3 optimizedWaveSources[32] = vec3[](
    vec3(-726.0, 789.0, -672.0),
    vec3(-564.0, -673.0, -351.0),
    vec3(-83.0, -3.0, 631.0),
    vec3(-775.0, -361.0, 755.0),
    vec3(-446.0, -134.0, -390.0),
    vec3(-219.0, -365.0, -135.0),
    vec3(-390.0, -316.0, -62.0),
    vec3(-78.0, -344.0, -678.0),
    vec3(515.0, 745.0, -173.0),
    vec3(-202.0, 752.0, -36.0),
    vec3(238.0, -775.0, -48.0),
    vec3(366.0, -538.0, 80.0),
    vec3(-786.0, 180.0, -724.0),
    vec3(645.0, 205.0, -284.0),
    vec3(599.0, 559.0, 383.0),
    vec3(-592.0, -460.0, -783.0),
    vec3(74.0, -50.0, -298.0),
    vec3(-788.0, 673.0, 159.0),
    vec3(-666.0, 387.0, 103.0),
    vec3(-39.0, -616.0, 55.0),
    vec3(726.0, 422.0, 81.0),
    vec3(-123.0, -12.0, 343.0),
    vec3(757.0, 2.0, -278.0),
    vec3(-767.0, -153.0, 728.0),
    vec3(-250.0, -355.0, 486.0),
    vec3(-668.0, -146.0, -774.0),
    vec3(-650.0, 729.0, -23.0),
    vec3(-148.0, 742.0, -151.0),
    vec3(-789.0, -725.0, -565.0),
    vec3(115.0, 37.0, -380.0),
    vec3(-631.0, -38.0, -758.0),
    vec3(250.0, 640.0, 30.0)
    );

//wavedx(vec3 uv, vec3 emitter, float speed, float phase, float timeshift)
vec2 wavetadalala(vec3 position, vec3 direction, float speed, float frequency, float timeshift) {
    float x = dot(direction, position) * frequency + timeshift * speed;
    float wave = exp(sin(x) - 1.0);
    float dx = wave * cos(x);
    return vec2(wave, dx);
}
float getwavesHighPhase(vec3 position, int iterations, float dragmult, float timeshift, float seed){
    timeshift *= 100.0;
    float seedWaves = seed;
    position *= 0.4 + aBitBetterNoise(position * 0.01)*0.02;
    float phase = 6.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    float scaling = 0.4;
    for(int i=0;i<iterations;i++){
        //vec3 p = normalize(vec3(oct(seedWaves += 1.0), oct(seedWaves += 1.0), oct(seedWaves += 1.0)) * 2.0 - 1.0);
        vec3 p = normalize(optimizedWaveSources[i]);
        vec2 res = wavetadalala(position, p, speed, phase * scaling, timeshift);
        position -= p * (weight) * (res.y) * 0.148;
        w += res.x * weight;
        ws += weight;
        weight = mix(weight, 0.0, 0.18);
        phase *= 1.2;
        speed *= 1.07;
    }
    return w / ws;
}
float getwavesHighPhase3(vec3 position, int iterations, float dragmult, float timeshift, float seed){
    float seedWaves = seed;
    float phase = 6.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<iterations;i++){
        vec3 p = normalize(vec3(oct(seedWaves += 1.0), oct(seedWaves += 1.0), oct(seedWaves += 1.0)) * 2.0 - 1.0);
        //vec3 p = normalize(optimizedWaveSources[i]);
        vec2 res = wavetadalala(position, p, speed, phase, timeshift);
        position -= p * (weight) * (res.y) * dragmult;
        w += res.x * weight;
        ws += weight;
        weight = mix(weight, 0.0, 0.18);
        phase *= 1.2;
        speed *= 1.07;
    }
    return w / ws;
}

float wavetada(vec3 position, vec3 direction, float speed, float frequency, float timeshift) {
    return exp(sin(dot(direction, position) * frequency + timeshift * speed * frequency) - 1.0);
}


float getwavesHighPhase2(vec3 position, int iterations, float dragmult, float timeshift, float seed){
    float seedWaves = seed;
    float phase = 6.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    vec3 displacevector = position * 0.001;
    for(int i=0;i<iterations;i++){
        //vec3 p = normalize(vec3(noise3d(0.000015 * position * (seedWaves += 10.0)), noise3d(0.000015 * position * (seedWaves += 10.0)), noise3d(0.000015 * position * (seedWaves += 10.0))) * 2.0 - 1.0);
        //vec3 p = normalize(vec3(sin(oct(seedWaves += 1.0) * 12.987852 + displacevector.x), sin(oct(seedWaves += 1.0) * 12.987852 + displacevector.y), sin(oct(seedWaves += 1.0) * 12.987852 + displacevector.z)));
        vec3 p = normalize(vec3(oct(seedWaves += 1.0), oct(seedWaves += 1.0), oct(seedWaves += 1.0)) * 2.0 - 1.0);
        float res = wavetada(position, p, speed, phase * 1.0, timeshift * 0.5);
        //float res2 = wavetada(position, p, speed, phase * 1.0, timeshift * 0.5 + 0.0006);
        //position -= normalize(position - p) * weight * pow(res - res2, 2.0) * 0.048;
        w += res * weight;
        ws += weight;
        weight = mix(weight, 0.01, 0.1);
        phase *= 1.26;
        speed *= 1.02;
    }
    return w / ws;
}
