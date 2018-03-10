#pragma once

vec3 getAtmosphereColorForRay(RenderPass pass, vec3 pos, vec3 dir){
    vec3 dirToStar = normalize(ClosestStarPosition - pos);
    vec3 normal = normalize(pos - pass.body.position);
    dir = reflect(pass.ray.d, normal);
    if(dot(normal, dir) < 0.0) dir = -reflect(dir, normal);
    float dt = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(dir, dirToStar))));
    float dt2 = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(normal, dirToStar))));
    vec3 noonColor = 1.0 - pass.body.atmosphereAbsorbColor;
    vec3 sunsetColor = pass.body.atmosphereAbsorbColor;
    return ClosestStarColor * sunsetColor * dt2;
}

float flatsmoothstep(float start, float end, float val){
    return clamp((val - start)/(end - start), 0.0, 1.0);
}

vec3 getAtmosphereAmbienceColorForPosition(RenderPass pass, vec3 pos){
    vec3 dir = normalize(pos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pos);
    float dt2 = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(dir, dirToStar))));
    vec3 noonColor = 1.0 - pass.body.atmosphereAbsorbColor;
    vec3 sunsetColor = pass.body.atmosphereAbsorbColor;
    float altitude = distance(pos, pass.body.position);
    return ClosestStarColor * sunsetColor * dt2 * (1.0 - flatsmoothstep(pass.body.radius, pass.body.atmosphereRadius, altitude)) * pass.body.atmosphereHeight;
}

CelestialRenderResult renderAtmospherePath(RenderPass pass, vec3 start, vec3 end){
    vec3 noonColor = (1.0 - pass.body.atmosphereAbsorbColor) * ClosestStarColor;
    vec3 sunsetColor = (pass.body.atmosphereAbsorbColor) * ClosestStarColor;
    float density = pass.body.atmosphereAbsorbStrength;
    float coverage = 0.0;
    vec3 alphacolor = vec3(0.0);
    vec3 color = vec3(0.0);
    float stepsize = 1.0 / 25.0;
    float iter = 0.0;
    float radius = pass.body.radius;
    float atmoheight = pass.body.atmosphereHeight;
    float rayStarDt = dot(normalize(ClosestStarPosition - pass.ray.o), pass.ray.d);
    float mieCoeff = (1.0 / (1.0 + 990.0 * (1.0 - max(0.0, rayStarDt))));
    float rayleightCoeff = (1.0 / (1.0 + 12.1 * (  1.0 - (rayStarDt * 0.5 + 0.5 ))));
    float distmultiplier = distance(start, end);
    float dimmer = 1.0 / (1.0 + 10.0*distmultiplier);
    //float startShadow = getStarTerrainShadowAtPoint(pass.body, start);
    //float endShadow = getStarTerrainShadowAtPoint(pass.body, end);
    for(int i=0;i<25;i++){
        vec3 pos = mix(start, end, iter);
        float cdst = distance(pos, pass.body.position) - radius;
        float heightmix = 1.0 - cdst / atmoheight;
        float heightmix_middle = 1.0 - abs(heightmix * 2.0 - 1.0);
        vec3 normal = normalize(pos - pass.body.position);
        vec3 dirToStar = normalize(ClosestStarPosition - pos);
        float dt = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(normal, dirToStar))));
        float realshadow = getHighCloudsShadowAtPoint(pass.body, pos);
        color += realshadow * rayleightCoeff * (1.0 - coverage) * (distmultiplier) * heightmix * mix(noonColor, sunsetColor, dt) * dt * 100.0;
        float dt2 =  max(0.0, dot(pass.ray.d, dirToStar));
        color += realshadow * heightmix * dt2 * ( mieCoeff * 4000.0) * distmultiplier * mix(noonColor, sunsetColor, dt) * dt * (pass.isSurfaceHit || pass.isWaterHit ? 0.0 : 1.0) * (1.0 - coverage);
        float lowClouds = 0.0;//celestialGetCloudsRaycast(pass.body, pos).r * heightmix_middle;
        alphacolor += lowClouds * (1.0 - coverage) * (1.0 - heightmix) * mix(noonColor, sunsetColor, dt) * dt;
        coverage += lowClouds * 0.25 * heightmix_middle;
        coverage = clamp(coverage, 0.0, 1.0);
        iter += stepsize;
    }
    color *= stepsize * dimmer;
    vec3 normal = normalize(start - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - start);
    float dt = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(normal, dirToStar))));
    return CelestialRenderResult(vec4(color, 0.0), vec4( 0.0));
}

vec4 alphaMix(vec4 a, vec4 b){
    a.rgb = mix(a.rgb, b.rgb, b.a);
    return vec4(a.rgb, min(1.0, a.a + b.a));
}

CelestialRenderResult renderAtmosphere(RenderPass pass){
    float centerDistance = distance(pass.ray.o, pass.body.position);
    float radius = pass.body.radius - pass.body.terrainMaxLevel;
    float atmoradius = pass.body.atmosphereRadius;
    int hitcount = 0;
    CelestialRenderResult result = emptyAtmosphereResult;
    vec3 planetHit = pass.surfaceHitPos;
    if(pass.isSurfaceHit || pass.isWaterHit) {
        hitcount++;
    }
    if(pass.isSurfaceHit && pass.isWaterHit && pass.waterHit < pass.surfaceHit){
        planetHit = pass.waterHitPos;
    }
    if(pass.isSurfaceHit && pass.isWaterHit && pass.waterHit > pass.surfaceHit){
        planetHit = pass.surfaceHitPos;
    }
    if(pass.isSurfaceHit && !pass.isWaterHit){
        planetHit = pass.surfaceHitPos;
    }
    if(!pass.isSurfaceHit && pass.isWaterHit){
        planetHit = pass.waterHitPos;
    }
    if(pass.isAtmosphereHit) {
        hitcount++;
    }
    if(hitcount == 0){
        return result;
    }
    if(centerDistance < radius){
        return result;
    }
    else if(centerDistance < atmoradius){
        if(hitcount == 1){
            result = renderAtmospherePath(pass, pass.ray.o, pass.atmosphereFarHitPos);
            vec4 hclouds = getHighClouds(pass.body, pass.atmosphereFarHitPos);
            result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
        }
        else if(hitcount == 2){
            result = renderAtmospherePath(pass, pass.ray.o, planetHit);
        }
    } else {
        if(hitcount == 1){
            result = renderAtmospherePath(pass, pass.atmosphereNearHitPos, pass.atmosphereFarHitPos);
            vec4 hclouds = getHighClouds(pass.body, pass.atmosphereNearHitPos);
            result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
        }
        else if(hitcount == 2){
            result = renderAtmospherePath(pass, pass.atmosphereNearHitPos, planetHit);
            vec4 hclouds = getHighClouds(pass.body, pass.atmosphereNearHitPos);
            result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
            //result.alphaBlendedLight.a = 0.0;//max(hclouds.a, result.alphaBlendedLight.a);
        }
    }
    return result;
}

float fresnelCoefficent(vec3 surfaceDir, vec3 incomingDir, float baseReflectivity){
    return (baseReflectivity + (1.0 - baseReflectivity) * (pow(1.0 - max(0.0, dot(surfaceDir, -incomingDir)), 5.0)));
}

vec3 renderWater(RenderPass pass){
    vec3 dirToStar = normalize(ClosestStarPosition - pass.waterHitPos);
    vec3 flatnormal = normalize(pass.waterHitPos - pass.body.position);
    float flatdt = max(-0.1, dot(flatnormal, dirToStar));
    vec3 waternormal = celestialGetWaterNormalRaycast(pass.body,  0.000028, pass.waterHitPos);

    waternormal = normalize(waternormal);
    flatnormal = normalize(pass.waterHitPos - pass.body.position);
    float flatdt2 = max(0.0, dot(flatnormal, dirToStar));
    float roughness = mix(0.0, 1.0, clamp(1.0 - 1.0 / (1.0 + pass.waterHit * 50.0), 0.0, 1.0));
    float colormultiplier = 1.0 - roughness * roughness * 0.94;
    float phongMult = mix(442.0, 8.0, roughness);
    waternormal = mix(waternormal, flatnormal, roughness);
    float fresnel = fresnelCoefficent(waternormal, pass.ray.d, 0.04);
    vec3 reflectedAtmo = fresnel * getAtmosphereColorForRay(pass, pass.waterHitPos, normalize(reflect(pass.ray.d, waternormal)));
    vec3 reflected = normalize(reflect(pass.ray.d, waternormal));
    float refldt = max(0.0, dot(reflected, dirToStar));
    //vec3 result = fresnel * colormultiplier * 10.0 * vec3(0.0, 0.002, 0.006) * max(0.0, flatdt) + reflectedAtmo;
    vec3 result = reflectedAtmo;
    result += fresnel * colormultiplier * ClosestStarColor * pow(refldt, phongMult) * 100.0;
    return result;
}

CelestialRenderResult renderCelestialBodyLightAtmosphere(RenderPass pass){
    vec3 color = celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz;
    float height = celestialGetHeightRaycast(pass.body, pass.surfaceHitPos);
    vec3 normal = celestialGetNormalRaycast(pass.body, sqrt(sqrt(pass.surfaceHit + 1.0)) * 0.004, pass.surfaceHitPos);
    vec3 flatnormal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 ambienceMultiplier = pow(max(0.0, dot(flatnormal, normal)) * 0.9 + 0.1, 12.0) *  ClosestStarColor * 0.0004;
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    float flatdt = max(-0.1, dot(flatnormal, dirToStar));
    //dt = max(dt * smoothstep(-0.1, 0.0, flatdt), flatdt * 0.5);
    CelestialRenderResult atmo = renderAtmosphere(pass);

    if(pass.isSurfaceHit && pass.isWaterHit){
        vec3 surface = vec3(0.0);
        vec3 realSurfaceDir = normalize(pass.surfaceHitPos - pass.body.position);
        vec3 realWaterDir = normalize(pass.waterHitPos - pass.body.position);
        float heightAtDir = pass.body.radius - celestialGetHeight(pass.body, realSurfaceDir) * pass.body.terrainMaxLevel;
        float waterAtDir = getWaterHeightHiRes(pass.body, realWaterDir);
        vec3 posSurface = pass.body.position + realSurfaceDir * heightAtDir;
        vec3 posWater = pass.body.position + realWaterDir * waterAtDir;
        float realDistanceSurface = distance(pass.ray.o, posSurface);
        float realDistanceWater = distance(pass.ray.o, posWater);
        vec3 shadowpos = vec3(0.0);
        if(realDistanceSurface > realDistanceWater){
            surface = renderWater(pass) * getStarTerrainShadowAtPoint(pass.body, pass.waterHitPos) * getHighCloudsShadowAtPoint(pass.body, pass.waterHitPos);
            shadowpos = pass.waterHitPos;
        } else {
            surface = ClosestStarColor * pow(dt, 4.0) * color * getStarTerrainShadowAtPoint(pass.body, pass.surfaceHitPos) * getHighCloudsShadowAtPoint(pass.body, pass.surfaceHitPos)
                        + color * getAtmosphereAmbienceColorForPosition(pass, pass.surfaceHitPos) * ambienceMultiplier;
        }
        atmo.alphaBlendedLight = vec4(mix(surface, atmo.alphaBlendedLight.rgb, atmo.alphaBlendedLight.a), 1.0);
    } else if(pass.isWaterHit) {
        vec3 surface = renderWater(pass) * getStarTerrainShadowAtPoint(pass.body, pass.waterHitPos) * getHighCloudsShadowAtPoint(pass.body, pass.waterHitPos);
        atmo.alphaBlendedLight = vec4(mix(surface, atmo.alphaBlendedLight.rgb, atmo.alphaBlendedLight.a), 1.0);
    } else if(pass.isSurfaceHit) {
        vec3 surface = ClosestStarColor * pow(dt, 4.0) * color * getStarTerrainShadowAtPoint(pass.body, pass.surfaceHitPos) * getHighCloudsShadowAtPoint(pass.body, pass.surfaceHitPos)
                    + color * getAtmosphereAmbienceColorForPosition(pass, pass.surfaceHitPos) * ambienceMultiplier;
        atmo.alphaBlendedLight = vec4(mix(surface, atmo.alphaBlendedLight.rgb, atmo.alphaBlendedLight.a), 1.0);
    }
    atmo.alphaBlendedLight.a = clamp(atmo.alphaBlendedLight.a, 0.0, 1.0);

    float threshold = pass.body.radius * 3.0;
    vec4 summedAlpha = vec4(atmo.additionLight.rgb + atmo.alphaBlendedLight.rgb, min(1.0, atmo.alphaBlendedLight.a + length(atmo.additionLight.rgb)));
    vec4 reducedAdditive = vec4(0.0);
    float mixvalue = clamp(distance(pass.ray.o, pass.body.position) / threshold, 0.0, 1.0);
    mixvalue = smoothstep(0.8, 1.0, mixvalue);
    atmo.additionLight = mix(atmo.additionLight, reducedAdditive, mixvalue);
    atmo.alphaBlendedLight = mix(atmo.alphaBlendedLight, summedAlpha, mixvalue);
    return atmo;
}
