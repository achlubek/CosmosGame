#pragma once

struct RenderPass {
        Ray ray;
        RenderedCelestialBody body;
        float surfaceHit;
        float atmosphereNearHit;
        float atmosphereFarHit;
        float waterHit;
        vec3 surfaceHitPos;
        vec3 atmosphereNearHitPos;
        vec3 atmosphereFarHitPos;
        vec3 waterHitPos;
        bool isAtmosphereHit;
        bool isSurfaceHit;
        bool isWaterHit;
};

#define DISTANCE_INFINITY 99999999.0


float getwavesHighPhaseTerrainRefinement(vec3 position, float dragmult, float timeshift, float seed){
    float iter = 0.0;
    float seedWaves = seed;
    float phase = 2.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<15;i++){
        vec3 p = (vec3(oct(seedWaves += 1.0), oct(seedWaves += 1.0), oct(seedWaves += 1.0)) * 2.0 - 1.0) * 300.0;
        float res = wave(position, p, speed, phase, 0.0 + timeshift);
        float res2 = wave(position, p, speed, phase, 0.006 + timeshift);
        position -= normalize(position - p) * (res - res2) * weight * dragmult;
        w += res * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.1);
        phase *= 1.6;
        speed *= 1.02;
    }
    return w / ws;
}


float celestialGetHeight(vec3 direction){
    float primary = texture(heightMapImage, xyzToPolar(direction)).r;
    float secondary = FBM3(direction * 0.9, 12, 2.0, 0.66);
    float refinement = pow(getwavesHighPhaseTerrainRefinement(direction.xyz * 0.1, 1.0, 0.0, 0.0),1.0);
    //return primary * 30.0 + secondary;
    vec3 coord = normalize(direction) * 10.0;
    return 1.0 - pow(primary + secondary * 1.0, 3.0);
}

float celestialGetHeightRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetHeight(normalize(position - body.position));
}

float raymarchCelestialTerrain(Ray ray, float startDistance, sampler2D s, RenderedCelestialBody body, float limit){
    float dist = startDistance + limit;
    float maxheight2 = body.radius;// + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 lastPos = ray.o;
    for(int i=0;i<7000;i++){
        vec3 p = ray.o + ray.d * dist;
        vec3 dir = normalize(p - center);
        float dc = distance(p, center); // probe distance to planet center
        float surfaceHeight = body.radius - celestialGetHeight(dir) * body.terrainMaxLevel; // surface height at probe position
        float ds = dc - surfaceHeight; // probe altitude
        if(ds < limit) return dist;
        if(dc > maxheight2 ) return -0.01;
        dist += ds ;
        lastPos = p;
    }
    return -0.01;
}

vec4 celestialGetColorRoughnessForDirection(vec3 direction){
    return texture(baseColorImage, xyzToPolar(direction)).rgba;
}

vec4 celestialGetColorRoughnessRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetColorRoughnessForDirection(normalize(position - body.position));
}

vec2 celestialGetCloudsForDirection(vec3 direction){
    vec2 prerendered = textureBicubic(cloudsImage, xyzToPolar(direction)).rg;
    return prerendered;
}

vec2 celestialGetCloudsRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetCloudsForDirection(normalize(position - body.position));
}

vec3 celestialGetNormal(RenderedCelestialBody body, float dxrange, vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, dxrange);
    mat3 normrotmat2 = rotationMatrix(bitangdir, dxrange);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * (body.radius - celestialGetHeight(dir) * body.terrainMaxLevel);
    vec3 p2 = dir2 * (body.radius - celestialGetHeight(dir2) * body.terrainMaxLevel);
    vec3 p3 = dir3 * (body.radius - celestialGetHeight(dir3) * body.terrainMaxLevel);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}

vec3 celestialGetNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetNormal(body, dxrange, normalize(position - body.position));
}

float getWaterHeightHiRes(RenderedCelestialBody body, vec3 dir){
    return (body.radius - body.fluidMaxLevel) - (1.0 - getwavesHighPhase(dir * 391.0, 19, 2.0, Time, 0.0)) * 0.0005;
}
float getWaterHeightLowRes(RenderedCelestialBody body, vec3 dir){
    return (body.radius - body.fluidMaxLevel) - (1.0 - getwavesHighPhase(dir * 391.0, 9, 2.0, Time, 0.0)) * 0.0005;
}

vec3 celestialGetWaterNormal(RenderedCelestialBody body, float dxrange, dvec3 dir){
    dvec3 tangdir = normalize(cross(dir, dvec3(0.0, 1.0, 0.0)));
    dvec3 bitangdir = normalize(cross(tangdir, dir));
    dmat3 normrotmat1 = drotationMatrix(tangdir, dxrange);
    dmat3 normrotmat2 = drotationMatrix(bitangdir, dxrange);
    dvec3 dir2 = dmat3(normrotmat1) * dir;
    dvec3 dir3 = dmat3(normrotmat2) * dir;
    dvec3 p1 = dir * double(getWaterHeightHiRes(body, vec3(dir)));
    dvec3 p2 = dir2 * double(getWaterHeightHiRes(body, vec3(dir2)));
    dvec3 p3 = dir3 * double(getWaterHeightHiRes(body, vec3(dir3)));
    return vec3(normalize(cross(normalize(p3 - p1), normalize(p2 - p1))));
}

vec3 celestialGetWaterNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetWaterNormal(body, dxrange, normalize(position - body.position));
}

float raymarchCelestialWater(Ray ray, float startDistance, RenderedCelestialBody body, float limit){
    float dist = startDistance+limit;
    float maxheight1 = body.radius - body.fluidMaxLevel;
    float maxheight2 = body.radius - body.fluidMaxLevel - 0.001;// + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 lastPos = ray.o;
    bool firstiter = false;
    for(int i=0;i<700;i++){
        vec3 p = ray.o + ray.d * dist;
        vec3 dir = normalize(p - center);
        float dc = distance(p, center); // probe distance to planet center
        if(dc > maxheight1 ) return -0.01;
        //if(dc < maxheight2 ) return dist;
        float surfaceHeight = getWaterHeightLowRes(body, dir);
        float ds = dc - surfaceHeight; // probe altitude
        if(ds < limit) {
            if(firstiter){
                dist -= limit * 10.0;
                limit *= 0.001;
            } else {
                return dist;
            }
        }
        dist += max(limit, ds) ;
        lastPos = p;
    }
    return -0.01;
}


void updatePassHits(inout RenderPass pass){
    float hit_Surface = rsi2(pass.ray, pass.body.surfaceSphere).x;
    float hit_Surface2 = rsi2(pass.ray, pass.body.surfaceSphere).y;
    float cameradst = distance(pass.body.position, pass.ray.o);
    //if(cameradst < pass.body.radius * 4.0 ){
        hit_Surface = raymarchCelestialTerrain(pass.ray, hit_Surface > 0.0 && hit_Surface < DISTANCE_INFINITY ? hit_Surface : 0.0, heightMapImage, pass.body, 0.00001 * cameradst);
    //}
    float hit_Water = rsi2(pass.ray, pass.body.waterSphere).x;
    if(hit_Water < 0.2 ){
        hit_Water = raymarchCelestialWater(pass.ray, hit_Water, pass.body, 0.00001 * cameradst);
    }
    vec2 hits_Atmosphere = rsi2(pass.ray, pass.body.atmosphereSphere);
    if(hit_Surface > 0.0 && hit_Surface < DISTANCE_INFINITY) {
        pass.isSurfaceHit = true;
        pass.surfaceHit = hit_Surface;
        pass.surfaceHitPos = pass.ray.o + pass.ray.d * pass.surfaceHit;
    }
    if(hit_Water > 0.0 && hit_Water < DISTANCE_INFINITY) {
        pass.isWaterHit = true;
        pass.waterHit = hit_Water;
        pass.waterHitPos = pass.ray.o + pass.ray.d * pass.waterHit;
    }
    if(hits_Atmosphere.y > 0.0 && hits_Atmosphere.y < DISTANCE_INFINITY) {
        pass.isAtmosphereHit = true;
        pass.atmosphereNearHit = hits_Atmosphere.x;
        pass.atmosphereFarHit = hits_Atmosphere.y;
        pass.atmosphereNearHitPos = pass.ray.o + pass.ray.d * pass.atmosphereNearHit;
        pass.atmosphereFarHitPos = pass.ray.o + pass.ray.d * pass.atmosphereFarHit;
    }
}

struct CelestialRenderResult
{
    vec4 additionLight;
    vec4 alphaBlendedLight;
};

CelestialRenderResult emptyAtmosphereResult = CelestialRenderResult(vec4(0.0), vec4(0.0));

#include celestialNoAtmosphere.glsl
#include celestialLightAtmosphere.glsl
#include celestialThickAtmosphere.glsl

CelestialRenderResult renderCelestialBody(RenderedCelestialBody body, Ray ray){
    RenderPass pass = RenderPass(ray, body, 0.0, 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), false, false, false);
    updatePassHits(pass);
    CelestialRenderResult result = emptyAtmosphereResult;
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_NO_ATMOSPHERE){
        if(pass.isSurfaceHit){
            result = renderCelestialBodyNoAtmosphere(pass);
        }
    }
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_LIGHT_ATMOSPHERE){
        if(pass.isAtmosphereHit){
            result = renderCelestialBodyLightAtmosphere(pass);
        }
    }
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_THICK_ATMOSPHERE){
        if(pass.isAtmosphereHit){
            result = renderCelestialBodyThickAtmosphere(pass);
        }
    }
    return result;
}
