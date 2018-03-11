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
    vec3 screenspace = clamp((orientedPoint / body.atmosphereRadius) * 0.5 + 0.5, 0.0, 1.0);
    screenspace.x = 1.0 - screenspace.x;
    screenspace.y = 1.0 - screenspace.y;
    return screenspace;
}

float getStarTerrainShadowAtPointShadow(RenderedCelestialBody body, vec3 point){
    vec3 coord = getShadowMapCoord(body, point);
    float shadowMapData = texture(shadowMapImage, coord.xy).r;
    return step(0.0, (coord.z) - shadowMapData);
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
    float maxheight2 = body.radius * 1.01;// + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 p = ray.o + ray.d * startDistance;
    for(int i=0;i<7000;i++){
        vec3 dir = normalize(p - center);
        float centerDistanceProbe = distance(p, center); // probe distance to planet center
        if(centerDistanceProbe > maxheight2 ) return -0.01;
        float centerDistanceSufrace = body.radius - celestialGetHeight(body, dir) * body.terrainMaxLevel; // surface height at probe position
        float altitude = centerDistanceProbe - centerDistanceSufrace; // probe altitude
        if(altitude < limit) return distance(center + centerDistanceSufrace * dir, ray.o);
        p += ray.d * max(limit, altitude * 0.3);
    }
    return -0.01;
}


float raymarchCelestialTerrainShadow(Ray ray, float startDistance, sampler2D s, RenderedCelestialBody body, float stepsize){
    float maxheight2 = body.radius * 1.01;// + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 p = ray.o + ray.d * startDistance;
    float visibility = 1.0;
    for(int i=0;i<7000;i++){
        vec3 dir = normalize(p - center);
        float centerDistanceProbe = distance(p, center); // probe distance to planet center
        if(centerDistanceProbe > maxheight2 ) return visibility;
        float centerDistanceSufrace = body.radius - celestialGetHeightLowRes(body, dir) * body.terrainMaxLevel; // surface height at probe position
        float altitude = centerDistanceProbe - centerDistanceSufrace; // probe altitude
        visibility -= max(0.0, (-altitude + 0.002) * 100.0);
        if(visibility < 0.0) return 0.0;
        p += ray.d * stepsize;
    }
    return visibility;
}

float getStarTerrainShadowAtPoint(RenderedCelestialBody body, vec3 point){
    vec3 dir = normalize(ClosestStarPosition - point);
    Ray ray = Ray(point, dir);
    vec2 surfaceHit = rsi2(ray, body.surfaceSphere);
    float startDistance = 0.0;
    if(distance(point, body.position) > body.radius){
        if(surfaceHit.x < 0.0 || surfaceHit.x >= DISTANCE_INFINITY) return 1.0;
        startDistance = surfaceHit.x;
    }
    float dst = raymarchCelestialTerrainShadow(ray, startDistance + 0.0001, heightMapImage, body, 0.001);
    //if(dst < 0.0) return 1.0;
    return dst;
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
    vec3 p1 = dir * (body.radius - celestialGetHeight(body, dir) * body.terrainMaxLevel);
    vec3 p2 = dir2 * (body.radius - celestialGetHeight(body, dir2) * body.terrainMaxLevel);
    vec3 p3 = dir3 * (body.radius - celestialGetHeight(body, dir3) * body.terrainMaxLevel);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}

vec3 celestialGetNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetNormal(body, dxrange, normalize(position - body.position));
}

float getWaterHeightHiRes(RenderedCelestialBody body, vec3 dir){
    dir = body.rotationMatrix * dir;
    return (body.radius - body.fluidMaxLevel) - (1.0 - getwavesHighPhase(dir * body.radius * 500.0, 24, 1.8, Time, 0.0)) * 0.0002;
}
float getWaterHeightLowRes(RenderedCelestialBody body, vec3 dir){
    dir = body.rotationMatrix * dir;
    return (body.radius - body.fluidMaxLevel) - (1.0 - getwavesHighPhase(dir * body.radius * 500.0, 8, 1.8, Time, 0.0)) * 0.0002;
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
    float maxheight1 = body.radius - body.fluidMaxLevel;
    float maxheight2 = body.radius - body.fluidMaxLevel + 0.005;// + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 p = ray.o + ray.d * startDistance;
    for(int i=0;i<700;i++){
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


vec4 getHighClouds(RenderedCelestialBody body, vec3 position){
    float highClouds = clamp(celestialGetCloudsRaycast(body, position).r * 1.0, 0.0, 1.0);
    vec3 dirToStar = normalize(ClosestStarPosition - position);
    vec3 normal = normalize(position - body.position);
    float dtv = max(0.0, dot(normal, dirToStar));
    float dt = 1.0 - (1.0 / (1.0 + 10.0 * dtv));
    vec3 sunsetColor = 1.0 - body.atmosphereAbsorbColor;
    vec3 color = mix(ClosestStarColor * 15.0, sunsetColor * 10.0, pow(1.0 - dtv, 12.0)) * dt;
    if(distance(vec3(0.0), body.position) < body.atmosphereRadius) color *= 0.5;
    return vec4(color, highClouds);
}

float getHighCloudsShadowAtPoint(RenderedCelestialBody body, vec3 point){
    vec3 dir = normalize(ClosestStarPosition - point);
    Ray ray = Ray(point, dir);
    vec2 atmoHit = rsi2(ray, body.atmosphereSphere);
    float startDistance = 0.0;
    if(distance(point, body.position) > body.atmosphereRadius){
        if(atmoHit.x < 0.0 || atmoHit.x >= DISTANCE_INFINITY) return 1.0;
    }
    vec3 newpos = ray.o + ray.d * atmoHit.y;
    return 0.1 + 0.9 * (1.0 - clamp(celestialGetCloudsForDirection(body, normalize(newpos - body.position)).r * 1.0, 0.0, 1.0));
}



void updatePassHits(inout RenderPass pass){
    float hit_Surface = rsi2(pass.ray, pass.body.surfaceSphere).x;
    float hit_Surface2 = rsi2(pass.ray, pass.body.surfaceSphere).y;
    float cameradst = distance(pass.body.position, pass.ray.o);
    if(cameradst < pass.body.radius * 4.0 ){
        hit_Surface = raymarchCelestialTerrain(pass.ray, hit_Surface > 0.0 && hit_Surface < DISTANCE_INFINITY ? hit_Surface : 0.0, heightMapImage, pass.body, 0.00001 );
    }
    float hit_Water = rsi2(pass.ray, pass.body.waterSphere).x;
    if(hit_Water < 0.08 && hit_Water > 0.0){
        hit_Water = raymarchCelestialWater(pass.ray, hit_Water, pass.body, 0.000001 * cameradst);
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

#ifndef SHADOW_MAP_COMPUTE_STAGE

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

#endif
