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


float celestialGetHeight(vec3 direction){
    float primary = texture(heightMapImage, xyzToPolar(direction)).r;
    float secondary = FBM3(direction * 150.0, 5, 3.0, 0.55);
    float refinement = pow(getwaves3d(direction.xyz, 1.0, 0.0),1.0);
    //return primary * 30.0 + secondary;
    vec3 coord = normalize(direction) * 10.0;
    seedWaves3d = 0.0;
    return primary ;
}

float celestialGetHeightRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetHeight(normalize(position - body.position));
}

float raymarchCelestialTerrain(Ray ray, float startDistance, sampler2D s, RenderedCelestialBody body, float limit){
    float dist = startDistance + limit;
    float maxheight2 = body.radius;// + body.terrainMaxLevel;
    float lastdst = DISTANCE_INFINITY;
    vec3 center = body.position;
    for(int i=0;i<7000;i++){
        vec3 p = ray.o + ray.d * dist;
        vec3 dir = normalize(p - center);
        float dc = distance(p, center);
        float ds = dc - (body.radius - celestialGetHeight(dir) * body.terrainMaxLevel);
        if(ds < limit) return dist;
        if(dc > maxheight2 ) return -0.01;
        lastdst = dc;
        dist += max(limit, ds * 0.77);
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
    vec3 p1 = dir * (body.radius + celestialGetHeight(dir) * body.terrainMaxLevel);
    vec3 p2 = dir2 * (body.radius + celestialGetHeight(dir2) * body.terrainMaxLevel);
    vec3 p3 = dir3 * (body.radius + celestialGetHeight(dir3) * body.terrainMaxLevel);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}

vec3 celestialGetNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetNormal(body, dxrange, normalize(position - body.position));
}

vec3 celestialGetWaterNormal(RenderedCelestialBody body, float dxrange, vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, dxrange);
    mat3 normrotmat2 = rotationMatrix(bitangdir, dxrange);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * ( getwaves3d(dir * 9910.0, 22.0, Time) * 0.001);
    vec3 p2 = dir2 * ( getwaves3d(dir2 * 9910.0, 22.0, Time) * 0.001);
    vec3 p3 = dir3 * ( getwaves3d(dir3 * 9910.0, 22.0, Time) * 0.001);
    return normalize(mix(dir, normalize(cross(normalize(p3 - p1), normalize(p2 - p1))), 0.1));
}

vec3 celestialGetWaterNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetWaterNormal(body, dxrange, normalize(position - body.position));
}

void updatePassHits(inout RenderPass pass){
    float hit_Surface = rsi2(pass.ray, pass.body.surfaceSphere).x;
    float hit_Surface2 = rsi2(pass.ray, pass.body.surfaceSphere).y;
    float cameradst = distance(pass.body.position, pass.ray.o);
    if(cameradst < pass.body.radius * 4.0 ){
        hit_Surface = raymarchCelestialTerrain(pass.ray, hit_Surface > 0.0 && hit_Surface < DISTANCE_INFINITY ? hit_Surface : 0.0, heightMapImage, pass.body, 0.000001 * cameradst);
    }
    float hit_Water = rsi2(pass.ray, pass.body.waterSphere).x;
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
