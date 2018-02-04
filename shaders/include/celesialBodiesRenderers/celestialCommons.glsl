#pragma once

struct RenderPass {
        Ray ray;
        RenderedCelestialBody body;
        float surfaceHit;
        float atmosphereNearHit;
        float atmosphereFarHit;
        vec3 surfaceHitPos;
        vec3 atmosphereNearHitPos;
        vec3 atmosphereFarHitPos;
        bool isAtmosphereHit;
        bool isSurfaceHit;
};

#define DISTANCE_INFINITY 99999999.0

float raymarchCelestialTerrain(Ray ray, sampler2D s, RenderedCelestialBody body, float limit){
    float dist = 0.0;
    float maxheight2 = body.radius + body.terrainMaxLevel;
    float lastdst = DISTANCE_INFINITY;
    vec3 center = body.position;
    for(int i=0;i<700;i++){
        vec3 p = ray.o + ray.d * dist;
        vec3 dir = normalize(p - center);
        float dc = distance(p, center);
        float ds = dc - (body.radius - texture(s, xyzToPolar(dir)).r * body.terrainMaxLevel);
        if(ds < limit) return dist;
        if(dc > maxheight2 && lastdst < dc) return -0.01;
        lastdst = dc;
        dist += ds * 0.3;
    }
    return -0.01;
}

float celestialGetHeight(vec3 direction){
    return texture(heightMapImage, xyzToPolar(direction)).r;
}

float celestialGetHeightRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetHeight(normalize(position - body.position));
}

vec4 celestialGetColorRoughnessForDirection(vec3 direction){
    vec3 pixel = vec3(1.0 / textureSize(baseColorImage, 0), 0.0) * 2.0;
    vec4 c1 = texture(baseColorImage, xyzToPolar(direction)).rgba;
    return c1;
}

vec4 celestialGetColorRoughnessRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetColorRoughnessForDirection(normalize(position - body.position));
}

vec3 celestialGetNormal(RenderedCelestialBody body, float dxrange, vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, dxrange);
    mat3 normrotmat2 = rotationMatrix(bitangdir, dxrange);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * (body.radius + celestialGetHeight(dir));
    vec3 p2 = dir2 * (body.radius + celestialGetHeight(dir2));
    vec3 p3 = dir3 * (body.radius + celestialGetHeight(dir3));
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}

vec3 celestialGetNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetNormal(body, dxrange, normalize(position - body.position));
}

void updatePassHits(inout RenderPass pass){
    float hit_Surface = rsi2(pass.ray, pass.body.surfaceSphere).x;
    if(distance(pass.body.position, pass.ray.o) < pass.body.radius * 4.0){
        hit_Surface = raymarchCelestialTerrain(pass.ray, heightMapImage, pass.body, 0.001);
    }
    vec2 hits_Atmosphere = rsi2(pass.ray, pass.body.atmosphereSphere);
    if(hit_Surface > 0.0 && hit_Surface < DISTANCE_INFINITY) {
        pass.isSurfaceHit = true;
        pass.surfaceHit = hit_Surface;
        pass.surfaceHitPos = pass.ray.o + pass.ray.d * pass.surfaceHit;
    }
    if(hits_Atmosphere.y > 0.0 && hits_Atmosphere.y < DISTANCE_INFINITY) {
        pass.isAtmosphereHit = true;
        pass.atmosphereNearHit = hits_Atmosphere.x;
        pass.atmosphereFarHit = hits_Atmosphere.y;
        pass.atmosphereNearHitPos = pass.ray.o + pass.ray.d * pass.atmosphereNearHit;
        pass.atmosphereFarHitPos = pass.ray.o + pass.ray.d * pass.atmosphereFarHit;
    }
}

#include celestialNoAtmosphere.glsl
#include celestialLightAtmosphere.glsl
#include celestialThickAtmosphere.glsl

vec4 renderCelestialBody(RenderedCelestialBody body, Ray ray){
    RenderPass pass = RenderPass(ray, body, 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), false, false);
    updatePassHits(pass);
    vec4 result = vec4(0.0);
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
