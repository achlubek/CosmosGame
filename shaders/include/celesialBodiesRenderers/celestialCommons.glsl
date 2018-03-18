#pragma once

struct RenderPass {
        Ray ray;
        RenderedCelestialBody body;
        float surfaceHit;
        float atmosphereNearHit;
        float atmosphereFarHit;
        float waterHit;
        float highCloudsHit;
        vec3 surfaceHitPos;
        vec3 atmosphereNearHitPos;
        vec3 atmosphereFarHitPos;
        vec3 waterHitPos;
        vec3 highCloudsHitPos;
        bool isAtmosphereHit;
        bool isSurfaceHit;
        bool isWaterHit;
        bool isHighCloudsHit;
};

#define DISTANCE_INFINITY 99999999.0


#ifndef SHADOW_MAP_COMPUTE_STAGE

vec3 getShadowMapCoord(RenderedCelestialBody body, vec3 point){
    /*
    float radius = body.atmosphereRadius;
    vec3 planeCenter = vec3(0.0, 0.0, radius);
    vec3 leftBottom = vec3(-radius, -radius, 0.0) + planeCenter;
    vec3 rightTop = vec3(radius, radius, 0.0) + planeCenter;
    vec3 planePoint = leftBottom + (rightTop - leftBottom) * vec3(UV, 0.0);
    vec3 direction = vec3(0.0, 0.0, -1.0);

    planePoint = body.fromHostToThisMatrix * planePoint;
    direction = body.fromHostToThisMatrix * direction;
    */
    mat3 inverseMatrix = (body.fromHostToThisMatrix);
    vec3 orientedPoint = inverseMatrix * (point - body.position);
    vec3 screenspace = clamp((orientedPoint / (body.atmosphereRadius)) * 0.5 + 0.5, 0.0, 1.0);
    //screenspace.x = 1.0 - screenspace.x;
    //screenspace.y = 1.0 - screenspace.y;
    return screenspace;
}

float textureShadowBlurBox(sampler2D tex, vec2 uv, int pixels){
    vec2 pixel = 1.0 / vec2(textureSize(tex, 0));
    float dw = 0.0;
    float dr = 0.0;
    for(int x = -pixels; x < pixels; x++){
        for(int y = -pixels; y < pixels; y++){
            dr += texture(tex, uv + vec2(x,y) * pixel).g;
            dw += 1.0;
        }
    }
    return dr / dw;
}

float getStarTerrainShadowAtPoint(RenderedCelestialBody body, vec3 point, float tolerance){
    vec3 dir = normalize(point - body.position);
    float probeheight = distance(point, body.position);
    vec2 data = texture(shadowMapImage, xyzToPolar( dir)).rg;
    vec3 coord = getShadowMapCoord(body, point);
    return smoothstep(-tolerance, 0.0, probeheight - (data.r + body.radius)) * texture(shadowMapImage, coord.xy).g;
}

float getStarTerrainShadowAtPointNoClouds(RenderedCelestialBody body, vec3 point){
    vec3 dir = normalize(point - body.position);
    float probeheight = distance(point, body.position);
    vec2 data = texture(shadowMapImage, xyzToPolar( dir)).rg;
    vec3 coord = getShadowMapCoord(body, point);
    return smoothstep(-0.001, 0.0, probeheight - (data.r + body.radius));
}

#else

float getStarTerrainShadowAtPoint(RenderedCelestialBody body, vec3 point, float tolerance){
    return 1.0;
}
float getStarTerrainShadowAtPointNoClouds(RenderedCelestialBody body, vec3 point){
    return 1.0;
}


#endif

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


float celestialGetHeight(RenderedCelestialBody body, vec3 direction){
    direction = body.rotationMatrix * direction;
    float primary = texture(heightMapImage, xyzToPolar(direction)).r;
    float secondary = abs(FBM3(direction * 220.9, 4, 2.0, 0.55) - 0.5);
    float refinement = pow(getwavesHighPhaseTerrainRefinement(direction.xyz * 0.1, 1.0, 0.0, 0.0),1.0);
    //return primary * 30.0 + secondary;
    vec3 coord = normalize(direction) * 10.0;
    return primary * 0.95 + secondary * 0.05;//smoothstep(0.99, 0.999, primary);
}
float celestialGetHeightLowRes(RenderedCelestialBody body, vec3 direction){
    float primary = texture(heightMapImage, xyzToPolar(body.rotationMatrix * direction)).r;
    return primary;
}

float celestialGetHeightRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetHeight(body, normalize(position - body.position));
}

float raymarchCelestialTerrain(Ray ray, float startDistance, sampler2D s, RenderedCelestialBody body, float limit){
    float maxheight2 = body.radius * 1.01 + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 p = ray.o + ray.d * startDistance;
    float waterLevel = (body.radius + body.fluidMaxLevel - 0.005);
    for(int i=0;i<700;i++){
        vec3 dir = normalize(p - center);
        float centerDistanceProbe = distance(p, center); // probe distance to planet center
        if(centerDistanceProbe > maxheight2 ) return -0.01;
        float centerDistanceSufrace = body.radius + celestialGetHeight(body, dir) * body.terrainMaxLevel; // surface height at probe position
        float altitude = centerDistanceProbe - centerDistanceSufrace; // probe altitude
        if(centerDistanceProbe < waterLevel) return distance(center + centerDistanceProbe * dir, ray.o);
        if(altitude < limit) return distance(center + centerDistanceSufrace * dir, ray.o);
        p += ray.d * max(limit, altitude * 0.8);
    }
    return -0.01;
}

float raymarchCelestialTerrainInternal(Ray ray, float startDistance, sampler2D s, RenderedCelestialBody body, float limit){
    float maxheight2 = body.atmosphereRadius * 1.01 + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 p = ray.o + ray.d * startDistance;
    float waterLevel = (body.radius + body.fluidMaxLevel);
    for(int i=0;i<700;i++){
        vec3 dir = normalize(p - center);
        float centerDistanceProbe = distance(p, center); // probe distance to planet center
        if(centerDistanceProbe > maxheight2 ) return -0.01;
        float centerDistanceSufrace = body.radius + celestialGetHeight(body, dir) * body.terrainMaxLevel; // surface height at probe position
        float altitude = centerDistanceProbe - centerDistanceSufrace; // probe altitude
        if(centerDistanceProbe < waterLevel) return distance(center + waterLevel * dir, ray.o);
        if(altitude < limit) return distance(center + centerDistanceSufrace * dir, ray.o);
        p += ray.d * max(limit, altitude * 0.8);
    }
    return -0.01;
}

vec4 celestialGetColorRoughnessForDirection(RenderedCelestialBody body, vec3 direction){
    return texture(baseColorImage, xyzToPolar(body.rotationMatrix * direction)).rgba;
}

vec4 celestialGetColorRoughnessRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetColorRoughnessForDirection(body, normalize(position - body.position));
}

vec2 celestialGetCloudsForDirection(RenderedCelestialBody body,vec3 direction){
    vec2 prerendered = textureBicubic(cloudsImage, xyzToPolar(body.rotationMatrix * direction)).rg;
    return prerendered;
}

vec2 celestialGetCloudsRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetCloudsForDirection(body, normalize(position - body.position));
}

vec3 celestialGetNormal(RenderedCelestialBody body, float dxrange, vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, dxrange);
    mat3 normrotmat2 = rotationMatrix(bitangdir, dxrange);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * (body.radius + celestialGetHeight(body, dir) * body.terrainMaxLevel);
    vec3 p2 = dir2 * (body.radius + celestialGetHeight(body, dir2) * body.terrainMaxLevel);
    vec3 p3 = dir3 * (body.radius + celestialGetHeight(body, dir3) * body.terrainMaxLevel);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}

vec3 celestialGetNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetNormal(body, dxrange, normalize(position - body.position));
}

float getWaterHeightHiRes(RenderedCelestialBody body, vec3 dir){
    dir = body.rotationMatrix * dir;
    return (body.radius + body.fluidMaxLevel) - (1.0 - getwavesHighPhase(dir * 2000.0, 16, 1.8, Time, 0.0)) * 0.0004;
}
float getWaterHeightLowRes(RenderedCelestialBody body, vec3 dir){
    dir = body.rotationMatrix * dir;
    return (body.radius + body.fluidMaxLevel) - (1.0 - getwavesHighPhase(dir * 2000.0, 5, 1.8, Time, 0.0)) * 0.0004;
}

vec3 celestialGetWaterNormal(RenderedCelestialBody body, float dxrange, vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, dxrange);
    mat3 normrotmat2 = rotationMatrix(bitangdir, dxrange);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * getWaterHeightHiRes(body, vec3(dir));
    vec3 p2 = dir2 * getWaterHeightHiRes(body, vec3(dir2));
    vec3 p3 = dir3 * getWaterHeightHiRes(body, vec3(dir3));
    return vec3(normalize(cross(normalize(p3 - p1), normalize(p2 - p1))));
}

vec3 celestialGetWaterNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetWaterNormal(body, dxrange, normalize(position - body.position));
}

float raymarchCelestialWater(Ray ray, float startDistance, RenderedCelestialBody body, float limit){
    float maxheight1 = body.radius + body.fluidMaxLevel + 0.005;
    float maxheight2 = body.radius + body.fluidMaxLevel + 0.0001;// + 0.0004;// + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 p = ray.o + ray.d * startDistance;
    for(int i=0;i<100;i++){
        vec3 dir = normalize(p - center);
        float centerDistanceProbe = distance(p, center); // probe distance to planet center
        if(centerDistanceProbe > maxheight2 ) return -0.01;
        float centerDistanceSufrace = getWaterHeightLowRes(body, dir);
        float altitude = centerDistanceProbe - centerDistanceSufrace; // probe altitude
        if(altitude < limit) return distance(p, ray.o);
        p += ray.d * max(limit, altitude*0.4);
    }
    return distance(p, ray.o);
}


void updatePassHits(inout RenderPass pass){
    float hit_Surface = rsi2(pass.ray, pass.body.surfaceSphere).x;
    float hit_Surface2 = rsi2(pass.ray, pass.body.surfaceSphere).y;
    float cameradst = distance(pass.body.position, pass.ray.o);
//    if(cameradst < pass.body.radius * 4.0 ){
        hit_Surface = raymarchCelestialTerrain(pass.ray, hit_Surface > 0.0 && hit_Surface < DISTANCE_INFINITY ? hit_Surface : 0.0, heightMapImage, pass.body, 0.00001 );
//    }
    float hit_Water = rsi2(pass.ray, pass.body.waterSphere).x;
    if(hit_Water < 0.08 && hit_Water > 0.0){
        hit_Water = raymarchCelestialWater(pass.ray, hit_Water, pass.body, 0.000001);
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
    vec2 hits_HighClouds = rsi2(pass.ray, pass.body.highCloudsSphere);
    float hit_HighClouds = (hits_HighClouds.x > 0.0 && hits_HighClouds.x < DISTANCE_INFINITY) ? hits_HighClouds.x : hits_HighClouds.y;
    if(hit_HighClouds > 0.0 && hit_HighClouds < DISTANCE_INFINITY) {
        pass.isHighCloudsHit = true;
        pass.highCloudsHit = hit_HighClouds;
        pass.highCloudsHitPos = pass.ray.o + pass.ray.d * pass.highCloudsHit;
    }
}

struct CelestialRenderResult
{
    vec4 additionLight;
    vec4 alphaBlendedLight;
};

CelestialRenderResult emptyAtmosphereResult = CelestialRenderResult(vec4(0.0), vec4(0.0));

#ifndef SHADOW_MAP_COMPUTE_STAGE

#include celestialNoAtmosphere.glsl
#include celestialLightAtmosphere.glsl
#include celestialThickAtmosphere.glsl

CelestialRenderResult renderCelestialBody(RenderedCelestialBody body, Ray ray){
    RenderPass pass = RenderPass(ray, body, 0.0, 0.0, 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), false, false, false, false);
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
    //result.additionLight = vec4(0.0);
//    result.alphaBlendedLight = vec4(1000.0 * texture(shadowMapImage, xyzToPolar( normalize(pass.surfaceHitPos - body.position))).rrr, result.alphaBlendedLight.a);
    return result;
}

#endif
