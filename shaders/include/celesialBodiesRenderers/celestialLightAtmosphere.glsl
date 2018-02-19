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
    return sunsetColor * dt2;
}

CelestialRenderResult renderAtmospherePath(RenderPass pass, vec3 start, vec3 end){
    vec3 noonColor = 1.0 - pass.body.atmosphereAbsorbColor;
    vec3 sunsetColor = pass.body.atmosphereAbsorbColor;
    float density = pass.body.atmosphereAbsorbStrength;
    float coverage = 0.0;
    vec3 alphacolor = vec3(0.0);
    vec3 color = vec3(0.0);
    float stepsize = 1.0 / 100.0;
    float iter = 0.0;
    float radius = pass.body.radius;
    float atmoheight = pass.body.atmosphereHeight;
    float rayStarDt = dot(normalize(ClosestStarPosition - pass.ray.o), pass.ray.d);
    float mieCoeff = (1.0 / (0.1 + 990.0 * (1.0 - max(0.0, rayStarDt))));
    float rayleightCoeff = (1.0 / (1.0 + 16.1 * (  1.0 - (rayStarDt * 0.5 + 0.5 ))));
    float distmultiplier = distance(start, end);
    for(int i=0;i<100;i++){
        vec3 pos = mix(start, end, iter);
        float cdst = distance(pos, pass.body.position) - radius;
        float heightmix = 1.0 - cdst / atmoheight;
        float heightmix_middle = 1.0 - abs(heightmix * 2.0 - 1.0);
        vec3 normal = normalize(pos - pass.body.position);
        vec3 dirToStar = normalize(ClosestStarPosition - pos);
        float dt = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(normal, dirToStar))));
        color += rayleightCoeff * (1.0 - coverage) * ClosestStarColor * 452.0 * sqrt(distmultiplier) * heightmix * mix(noonColor, sunsetColor, dt) * dt * dt;
        float dt2 =  max(0.0, dot(pass.ray.d, dirToStar));
        color += heightmix * dt2 * ( mieCoeff * 4000.0) * distmultiplier * mix(noonColor, sunsetColor, dt) * dt * (pass.isSurfaceHit ? 0.0 : 1.0) * (1.0 - coverage);
        float lowClouds = celestialGetCloudsRaycast(pass.body, pos).r * heightmix_middle;
        alphacolor += (1.0 - coverage) * (1.0 - heightmix) * mix(noonColor, sunsetColor, dt) * dt;
        coverage += lowClouds * 0.25 * heightmix_middle;
        coverage = clamp(coverage, 0.0, 1.0);
        iter += stepsize;
    }
    color *= stepsize;
    vec3 normal = normalize(start - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - start);
    float dt = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(normal, dirToStar))));
    return CelestialRenderResult(vec4(color, 0.0), vec4( alphacolor,coverage));
}

vec4 getHighClouds(RenderedCelestialBody body, vec3 position){
    float highClouds = celestialGetCloudsRaycast(body, position).g;
    vec3 dirToStar = normalize(ClosestStarPosition - position);
    vec3 normal = normalize(position - body.position);
    float dt = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(normal, dirToStar))));
    vec3 sunsetColor = body.atmosphereAbsorbColor;
    vec3 color = mix(vec3(10.0), sunsetColor, dt) * dt;
    return vec4(color * highClouds, highClouds) ;
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
            result.alphaBlendedLight += getHighClouds(pass.body, pass.atmosphereFarHitPos);
        }
        else if(hitcount == 2){
            result = renderAtmospherePath(pass, pass.ray.o, planetHit);
        }
    } else {
        if(hitcount == 1){
            result = renderAtmospherePath(pass, pass.atmosphereNearHitPos, pass.atmosphereFarHitPos);
            result.alphaBlendedLight += getHighClouds(pass.body, pass.atmosphereNearHitPos);
        }
        else if(hitcount == 2){
            result = renderAtmospherePath(pass, pass.atmosphereNearHitPos, planetHit);
            result.alphaBlendedLight += getHighClouds(pass.body, pass.atmosphereNearHitPos);
        }
    }
    return result;
}

CelestialRenderResult renderCelestialBodyLightAtmosphere(RenderPass pass){
    vec3 color = celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz;
    float height = celestialGetHeightRaycast(pass.body, pass.surfaceHitPos);
    vec3 normal = celestialGetNormalRaycast(pass.body, sqrt(sqrt(pass.surfaceHit + 1.0)) * 0.004, pass.surfaceHitPos);
    vec3 flatnormal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    float flatdt = max(-0.1, dot(flatnormal, dirToStar));
    dt = max(dt * smoothstep(-0.1, 0.0, flatdt), flatdt * 0.5);
    CelestialRenderResult atmo = renderAtmosphere(pass);
    if(pass.isSurfaceHit && pass.isWaterHit){
        vec3 surface = vec3(0.0);
        if(pass.waterHit < pass.surfaceHit){
            vec3 waternormal = celestialGetWaterNormalRaycast(pass.body,  0.000004, pass.waterHitPos);
            float refldt = max(0.0, dot(normalize(reflect(pass.ray.d, waternormal)), dirToStar));
            vec3 reflectedAtmo = getAtmosphereColorForRay(pass, pass.waterHitPos, normalize(reflect(pass.ray.d, waternormal)));
            surface = 10.0 * vec3(0.0, 0.02, 0.06) * max(0.0, flatdt) + reflectedAtmo;
            surface += 10.0 * ClosestStarColor * pow(refldt, 442.0);
        } else {
            surface = 10.0 * color * dt;
        }
        atmo.alphaBlendedLight = vec4(mix(surface, atmo.alphaBlendedLight.rgb, atmo.alphaBlendedLight.a), 1.0);
    } else if(pass.isWaterHit) {
        vec3 waternormal = celestialGetWaterNormalRaycast(pass.body,  0.000004, pass.waterHitPos);
        float refldt = max(0.0, dot(normalize(reflect(pass.ray.d, waternormal)), dirToStar));
        vec3 reflectedAtmo = getAtmosphereColorForRay(pass, pass.waterHitPos, normalize(reflect(pass.ray.d, waternormal)));
        vec3 surface = 10.0 * vec3(0.0, 0.02, 0.06) * max(0.0, flatdt) + reflectedAtmo;
        surface += 10.0 * ClosestStarColor * pow(refldt, 442.0);
        atmo.alphaBlendedLight = vec4(mix(surface, atmo.alphaBlendedLight.rgb, atmo.alphaBlendedLight.a), 1.0);
    }
    atmo.alphaBlendedLight.a = clamp(atmo.alphaBlendedLight.a, 0.0, 1.0);

    float threshold = pass.body.radius * 3.0;
    vec4 summedAlpha = vec4(atmo.additionLight.rgb + atmo.alphaBlendedLight.rgb, 1.0);
    vec4 reducedAdditive = vec4(0.0);
    float mixvalue = clamp(distance(pass.ray.o, pass.body.position) / threshold, 0.0, 1.0);
    mixvalue = smoothstep(0.8, 1.0, mixvalue);
    atmo.additionLight = mix(atmo.additionLight, reducedAdditive, mixvalue);
    atmo.alphaBlendedLight = mix(atmo.alphaBlendedLight, summedAlpha, mixvalue);
    return atmo;
}
