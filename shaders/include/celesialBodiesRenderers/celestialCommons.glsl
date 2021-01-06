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


vec2 celestialGetCloudsForDirection(RenderedCelestialBody body, vec3 direction){
    vec2 prerendered = textureBicubic(cloudsImage, xyzToPolar(body.rotationMatrix * direction)).rg;
    return prerendered;
}

vec2 celestialGetCloudsRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetCloudsForDirection(body, normalize(position - body.position));
}

float getStarTerrainShadowAtPoint(RenderedCelestialBody body, vec3 point, float tolerance){
    return 1.0;
    Ray ray = Ray(point, normalize(ClosestStarPosition - point));
    float waterSphereShadow = rsi2(ray, body.waterSphere).x;
    float highCloudsHit = rsi2(ray, body.highCloudsSphere).y;
    waterSphereShadow = hits(waterSphereShadow) ? 0.0 : 1.0;// smoothstep(-0.25, 0.0, dot(normalize(point - body.position), normalize(ClosestStarPosition - point)));

    vec3 opo = mat3(FromStarToThisMatrix) * (point / ShadowMapDivisors1);
    // dirty hacks
    float surfaceShadow = waterSphereShadow;
    if(tolerance > 0.0){
        if(length(opo.xy) < 0.94){
            float depthTexture = textureBicubic(shadowMap1, opo.xy * 0.5 + 0.5).r;
            surfaceShadow = 1.0 - smoothstep(-0.001, 0.03, ((opo.z * 0.5 + 0.5) - depthTexture));
        } else {
            opo = mat3(FromStarToThisMatrix) * (point / ShadowMapDivisors2);
            if(length(opo.xy) < 0.98){
                float depthTexture = textureBicubic(shadowMap2, opo.xy * 0.5 + 0.5).r;
                surfaceShadow = 1.0 - smoothstep(-0.001, 0.03, ((opo.z * 0.5 + 0.5) - depthTexture));
            } else {
                opo = mat3(FromStarToThisMatrix) * (point / ShadowMapDivisors3);
                if(length(opo.xy) < 0.98){
                    float depthTexture = textureBicubic(shadowMap3, opo.xy * 0.5 + 0.5).r;
                    surfaceShadow = 1.0 - smoothstep(-0.001, 0.03, ((opo.z * 0.5 + 0.5) - depthTexture));
                }
            }
        }
    }
    float cloudsShadow = 1.0;
    if(highCloudsHit > 0.001 && highCloudsHit < DISTANCE_INFINITY && tolerance > 0.0){
        vec3 cloudsPos = ray.o + ray.d * highCloudsHit;
        cloudsShadow = 1.0 - celestialGetCloudsRaycast(body, cloudsPos).x;
    }
    return surfaceShadow * cloudsShadow;//temouv.x < 0.5 ? depthTexture : (opo.z * 0.5 + 0.5);
}

float getStarTerrainShadowAtPointNoClouds(RenderedCelestialBody body, vec3 point){
    return getStarTerrainShadowAtPoint(body, point, 0.0);//smoothstep(-0.001, 0.0, probeheight - (data.r + body.radius));
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
    return primary;
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

vec4 celestialGetColorRoughnessForDirection(RenderedCelestialBody body, vec3 direction){
    return texture(baseColorImage, xyzToPolar(body.rotationMatrix * direction)).rgba;
}

vec4 celestialGetColorRoughnessRaycast(RenderedCelestialBody body, vec3 position){
    return celestialGetColorRoughnessForDirection(body, normalize(position - body.position));
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
//    dir = body.rotationMatrix * dir;
//    float waterHeightHere = noise4d(vec4(dir * 14.0, Time * 0.01));
    float wheight = 0.002 + 0.056;// * waterHeightHere;
    float b = FBM4(vec4(dir * body.radius * 250.01, Time * 0.1), 4, 3.0, 0.5) * wheight;
    return (body.radius + body.fluidMaxLevel) - (1.0 - getwavesHighPhase(dir * body.radius * 150.01, 23, 1.8, Time * 0.01, 0.0)) * wheight * 0.1 - b * 0.02;
}
float getWaterHeightLowRes(RenderedCelestialBody body, vec3 dir){
    //dir = body.rotationMatrix * dir;
    //float waterHeightHere = noise4d(vec4(dir * 14.0, Time * 0.01));
    float wheight = 0.002 + 0.056;// * waterHeightHere;
    return (body.radius + body.fluidMaxLevel) - (1.0 - getwavesHighPhase(dir * body.radius * 150.01, 15, 1.8, Time * 0.01, 0.0)) * wheight * 0.1;
}

vec3 celestialGetWaterNormal(RenderedCelestialBody body, float dxrange, vec3 dir){
    //dir = body.rotationMatrix * dir;
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, dxrange);
    mat3 normrotmat2 = rotationMatrix(bitangdir, dxrange);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * getWaterHeightHiRes(body, vec3(dir));
    vec3 p2 = dir2 * getWaterHeightHiRes(body, vec3(dir2));
    vec3 p3 = dir3 * getWaterHeightHiRes(body, vec3(dir3));
    vec3 n =  normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
    return normalize(n);

}

vec3 celestialGetWaterNormalRaycast(RenderedCelestialBody body, float dxrange, vec3 position){
    return celestialGetWaterNormal(body, dxrange, normalize(position - body.position));
}

float raymarchCelestialWater(Ray ray, float startDistance, RenderedCelestialBody body, float limit){
    float maxheight = body.radius + body.fluidMaxLevel + 0.01;// + 0.0004;// + body.terrainMaxLevel;
    vec3 center = body.position;
    vec3 p = ray.o + ray.d * startDistance;
    for(int i=0;i<17;i++){
        vec3 dir = normalize(p - center);
        float centerDistanceProbe = distance(p, center); // probe distance to planet center
        if(centerDistanceProbe > maxheight ) return -0.01;
        float centerDistanceSufrace = getWaterHeightLowRes(body, dir);
        float altitude = centerDistanceProbe - centerDistanceSufrace; // probe altitude
        if(altitude < limit) return distance(p, ray.o);
        p += ray.d * max(limit, altitude*0.7);
    }
    return distance(p, ray.o);
}


void updatePassHits(inout RenderPass pass){
    float hit_Surface = 0.0;//rsi2(pass.ray, pass.body.surfaceSphere).x;
    float hit_Surface2 = 0.0;//rsi2(pass.ray, pass.body.surfaceSphere).y;
    float cameradst = distance(pass.body.position, pass.ray.o);
//    if(cameradst < pass.body.radius * 4.0 ){
    vec2 uv = vec2(0.0);
#ifndef SHADOW_MAP_COMPUTE_STAGE
    uv = gl_FragCoord.xy / Resolution;
    hit_Surface = texture(surfaceRenderedDistanceImage, uv).r;//raymarchCelestialTerrain(pass.ray, hit_Surface > 0.0 && hit_Surface < DISTANCE_INFINITY ? hit_Surface : 0.0, heightMapImage, pass.body, 0.00001 );
#endif
//    }

    float hit_Water_Spherical = rsi2(pass.ray, pass.body.waterSphere).x;
    float hit_Water = 0.0;
    if(hit_Water_Spherical > 0.0){
        hit_Water = raymarchCelestialWater(pass.ray, hit_Water_Spherical, pass.body, 0.000001);
        hit_Water = mix(hit_Water, hit_Water_Spherical, clamp(hit_Water_Spherical / 2.22, 0.0, 1.0)); //magic
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

float fresnelCoefficent(vec3 surfaceDir, vec3 incomingDir, float baseReflectivity){
    return (baseReflectivity + (1.0 - baseReflectivity) * (pow(1.0 - max(0.0, dot(surfaceDir, -incomingDir)), 5.0)));
}

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{
    return dot(point - origin, normal) / dot(direction, normal);
}

CelestialRenderResult renderRings(RenderPass pass, CelestialRenderResult currentResult){
    if(oct(pass.body.seed) < 0.95) return currentResult;
    vec3 planepoint = pass.body.position;
    vec3 planenormal = normalize(vec3(oct(pass.body.seed) * 2.0 - 1.0, 5.0, oct(pass.body.seed + 100.0) * 2.0 - 1.0));
    float hit = intersectPlane(pass.ray.o, pass.ray.d, planepoint, planenormal);
    if(hit <= 0.0 || hit > 99999.0) return currentResult;
    vec3 pos = pass.ray.o + pass.ray.d * hit;
    float centerDistance = distance(pos, planepoint);
    float start =  mix(1.2, 7.0, oct(pass.body.seed + 10.0));
    float stop = start + mix(0.5, 2.5, oct(pass.body.seed + 20.0));
    float frequency = 0.1 + 2.0 * oct(pass.body.seed + 30.0) / pass.body.radius;
    float begin = pass.body.radius * start;
    float end = pass.body.radius * stop;
    float falloffBegin = 5.1 + 8.4 * oct(pass.body.seed + 40.0);
    float falloffEnd = 5.1 + 8.4 * oct(pass.body.seed + 50.0);
    float coverage = smoothstep(begin, begin + falloffBegin, centerDistance) * (1.0 - smoothstep(end, end + falloffEnd, centerDistance));

    vec3 dirToStar = normalize(ClosestStarPosition - pos);
    float dt = abs(dot(planenormal, dirToStar));
    float waterhit = rsi2(Ray(pos, dirToStar), pass.body.waterSphere).x;
    vec3 hitnormal = normalize((pos + dirToStar * waterhit) - planepoint);
    float shadowdt = max(0.0, dot(-dirToStar, hitnormal));
    float shadow = waterhit > 0.0 && waterhit < 999999.0 ? smoothstep(0.6, 0.98, 1.0 - shadowdt) : 1.0;

    centerDistance += FBM3(pos - planepoint, 6, 2.0, 0.66);
    float coverageFlunctuations = FBM1(frequency * centerDistance, 6, 2.0, 0.66);
    vec3 color = pass.body.sufraceMainColor * vec3(FBM1(frequency * centerDistance + 100.0, 6, 2.0, 0.66), FBM1(frequency * centerDistance + 200.0, 6, 2.0, 0.66), FBM1(frequency * centerDistance + 300.0, 6, 2.0, 0.66));
    color = mix(color, vec3(length(color)), 0.4);
    vec4 rings = vec4(shadow * ClosestStarColor * dt * color, min(1.0, coverage * coverageFlunctuations * 2.0));

    if(distance(pos, pass.ray.o) < distance(planepoint, pass.ray.o)){
        // rings in front of planet
        currentResult.alphaBlendedLight.rgb = mix(currentResult.alphaBlendedLight.rgb, rings.rgb, rings.a);
        currentResult.additionLight.rgb *= 1.0 - rings.a;
        currentResult.alphaBlendedLight.a = min(1.0, rings.a + currentResult.alphaBlendedLight.a);
    } else {
        // rings in the back
        currentResult.alphaBlendedLight.rgb = mix(rings.rgb * rings.a + currentResult.alphaBlendedLight.rgb, currentResult.alphaBlendedLight.rgb, currentResult.alphaBlendedLight.a);
        currentResult.alphaBlendedLight.a = min(1.0, rings.a + currentResult.alphaBlendedLight.a);
    }
    return currentResult;
}

float ringsShadow(RenderPass pass, Ray ray){
    if(oct(pass.body.seed) < 0.95) return 0.0;
    vec3 planepoint = pass.body.position;
    vec3 planenormal = normalize(vec3(oct(pass.body.seed) * 2.0 - 1.0, 5.0, oct(pass.body.seed + 100.0) * 2.0 - 1.0));
    float hit = intersectPlane(ray.o, ray.d, planepoint, planenormal);
    if(hit <= 0.0 || hit > 99999.0) return 0.0;
    vec3 pos = ray.o + ray.d * hit;
    float centerDistance = distance(pos, planepoint);
    float start =  mix(1.2, 7.0, oct(pass.body.seed + 10.0));
    float stop = start + mix(0.5, 2.5, oct(pass.body.seed + 20.0));
    float frequency = 0.1 + 2.0 * oct(pass.body.seed + 30.0) / pass.body.radius;
    float begin = pass.body.radius * start;
    float end = pass.body.radius * stop;
    float falloffBegin = 5.1 + 8.4 * oct(pass.body.seed + 40.0);
    float falloffEnd = 5.1 + 8.4 * oct(pass.body.seed + 50.0);
    float coverage = smoothstep(begin, begin + falloffBegin, centerDistance) * (1.0 - smoothstep(end, end + falloffEnd, centerDistance));

    centerDistance += FBM3(pos - planepoint, 6, 2.0, 0.66);
    float coverageFlunctuations = FBM1(frequency * centerDistance, 6, 2.0, 0.66);

    return coverage * coverageFlunctuations * 2.0;
}

#include celestialNoAtmosphere.glsl
#include celestialLightAtmosphere.glsl
#include celestialThickAtmosphere.glsl

CelestialRenderResult renderCelestialBody(RenderedCelestialBody body, Ray ray){
    RenderPass pass = RenderPass(ray, body, 0.0, 0.0, 0.0, 0.0, 0.0, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), false, false, false, false);
    updatePassHits(pass);
    CelestialRenderResult result = emptyAtmosphereResult;
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_NO_ATMOSPHERE){
        result = renderCelestialBodyNoAtmosphere(pass);
    }
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_LIGHT_ATMOSPHERE){
        result = renderCelestialBodyLightAtmosphere(pass);
    }
    if(body.renderMethod == CELESTIAL_RENDER_METHOD_THICK_ATMOSPHERE){
        result = renderCelestialBodyThickAtmosphere(pass);
    }
    result = renderRings(pass, result);
    //result.additionLight = vec4(0.0);
    //vec3 nrm = texture(surfaceRenderedAlbedoRoughnessImage, gl_FragCoord.xy / Resolution).rgb;
    //float ddd = texture(surfaceRenderedDistanceImage, gl_FragCoord.xy / Resolution).r;
    result.additionLight.a = 0.0;
    if(pass.isSurfaceHit) result.additionLight.a = length(pass.surfaceHitPos);
    if(pass.isWaterHit) result.additionLight.a = min(result.additionLight.a, length(pass.waterHitPos));
    //if(pass.isSurfaceHit) result.alphaBlendedLight = vec4(10000.0);
    //result.alphaBlendedLight = vec4(1.0, 1.0, 1.0, 0.01 + texture(surfaceRenderedDistanceImage, gl_FragCoord.xy / Resolution).r);
    //result.additionLight = vec4(10000.0);
    return result;
}

#endif
