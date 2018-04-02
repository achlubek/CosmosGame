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
    return ClosestStarColor * mix(noonColor, sunsetColor, dt);
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

float getAtmosphereAbsorptionMultiplier(RenderedCelestialBody body){
    return body.radius * 11000.0;
}

CelestialRenderResult renderAtmospherePath(RenderPass pass, vec3 start, vec3 end, float mieMultiplier, bool highQuality){
    //vec3 noonColor = (1.0 - pass.body.atmosphereAbsorbColor) * ClosestStarColor * 0.02;
    //vec3 sunsetColor = (pass.body.atmosphereAbsorbColor) * ClosestStarColor;
    float density = pass.body.atmosphereAbsorbStrength;
    float coverage = 0.0;
    vec3 alphacolor = vec3(0.0);
    vec3 color = vec3(0.0);
    float stepsize = 1.0 / 10.0;
    #ifdef SHADOW_MAP_COMPUTE_STAGE
    vec2 UV = vec2(0.0);
    #endif
    float iter = 0.0 + stepsize * fract(oct(UV * 100.0) + Time * 0.01);
    float radius = pass.body.radius;
    float atmoheight = pass.body.atmosphereHeight;
    vec3 starDir = normalize(ClosestStarPosition - start);
    vec3 direction = normalize(end - start);
    float dt2 = dot(direction, starDir);
    float rayStarDt = dot(starDir, direction);
    float mieCoeff = exp(-3.1415 * 5.0 * (-rayStarDt * 0.5 + 0.5)) * mieMultiplier;
    float rayleightCoeff =  exp(-0.1415 * (-rayStarDt * 0.5 + 0.5));//(1.0 / (1.0 + 12.1 * (  1.0 - (rayStarDt ))));
    float distmultiplier = distance(start, end);
    float dimmer = 1.0 / (1.0 + 10.0*distmultiplier);
    float absorbMultiplier = getAtmosphereAbsorptionMultiplier(pass.body);
    //float startShadow = getStarTerrainShadowAtPoint(pass.body, start);
    //float endShadow = getStarTerrainShadowAtPoint(pass.body, end);
    float shadowAccumulator = 0.0;
    if(highQuality){
        float stepsizeShadows = 1.0 / 17.0;
        float iterShadows = stepsizeShadows * fract(oct(UV * 100.0) + Time * 0.01) * 0.999 / (1.0 + max(0.0, distmultiplier * 0.1)) ;
        for(int i=0;i<17;i++){
            Ray secondaryRay = Ray( mix(start, end, iterShadows), starDir);
            //shadowAccumulator += 1.0 - step(0.0, rsi2(secondaryRay, pass.body.waterSphere).x);
            shadowAccumulator += getStarTerrainShadowAtPoint(pass.body, mix(start, end, iterShadows), 1.0);
            iterShadows += stepsizeShadows;
        }
        shadowAccumulator *= stepsizeShadows;
        //    vec3 normal = normalize(start - pass.body.position);
        //    float dt = 1.0 - (1.0 / (1.0 + 3.0 * max(0.0, dot(normal, starDir) * 0.8 + 0.2)));
        //    shadowAccumulator = dt;
    } else {
        vec3 normal = normalize(start - pass.body.position);
        float dt = 1.0 - (1.0 / (1.0 + 3.0 * max(0.0, dot(normal, starDir))));
        shadowAccumulator = dt;
    }
    vec3 rayEnergy = ClosestStarColor;
    for(int i=0;i<10;i++){
        vec3 pos = mix(start, end, iter);
        float cdst = distance(pos, pass.body.position) - radius;
        float heightmix = 1.0 - cdst / atmoheight;
        vec3 rayleightScatteredLight = heightmix * pass.body.atmosphereAbsorbColor * rayEnergy * rayleightCoeff * distmultiplier * 1225.1 * stepsize;
        vec3 mieScatteredLight       = heightmix * pass.body.atmosphereAbsorbColor * rayEnergy * mieCoeff       * distmultiplier * 525.1 * stepsize;

        rayEnergy -= rayleightScatteredLight;
        rayEnergy -= mieScatteredLight;

        rayEnergy = max(vec3(0.0), rayEnergy);

        //color += rayleightScatteredLight * (1.0 - coverage) * shadowAccumulator;//shadowAccumulator * rayleightCoeff * (1.0 - coverage) * (distmultiplier) * heightmix * visibleLight * 10.0;
        color += (mieScatteredLight + rayleightScatteredLight) * (1.0 - coverage) * shadowAccumulator * distmultiplier;// * (pass.isSurfaceHit || pass.isWaterHit ? 0.0 : 1.0) * (1.0 - coverage);
        /*float lowClouds = celestialGetCloudsRaycast(pass.body, pos).g * heightmix_middle;
        alphacolor += lowClouds * (1.0 - coverage) * (1.0 - heightmix) * rayEnergy;
        coverage += lowClouds * 0.25 * heightmix_middle;
        coverage = clamp(coverage, 0.0, 1.0);*/
        iter += stepsize;
    }
    color *= stepsize * dimmer;
    return CelestialRenderResult(vec4(color, 0.0), vec4(alphacolor, coverage));
}

CelestialRenderResult getAtmosphereLightForRay(RenderPass pass, Ray ray, float mieMultiplier){
    float primaryLength = rsi2(ray, pass.body.atmosphereSphere).y;
    return renderAtmospherePath(pass, ray.o, ray.o + ray.d * primaryLength, mieMultiplier, false);
}

vec3 getSunColorForRay(RenderedCelestialBody body, Ray ray){
    float primaryLength = max(0.0, rsi2(ray, body.atmosphereSphere).y) * 0.1;
    return clamp(ClosestStarColor - primaryLength * getAtmosphereAbsorptionMultiplier(body) * body.atmosphereAbsorbColor, vec3(0.0), ClosestStarColor);
}
vec3 scatterLight(RenderedCelestialBody body, vec3 observer, vec3 point, vec3 light){
    float primaryLength = max(0.0, rsi2(Ray(point, normalize(observer - point)), body.atmosphereSphere).y);
    primaryLength = min(primaryLength, distance(observer, point)) * 0.1;
    return clamp(light - primaryLength * getAtmosphereAbsorptionMultiplier(body) * body.atmosphereAbsorbColor, vec3(0.0), light);
}

vec4 alphaMix(vec4 a, vec4 b){
    a.rgb = mix(a.rgb, b.rgb, b.a);
    return vec4(a.rgb, min(1.0, a.a + b.a));
}

vec4 getHighClouds(RenderedCelestialBody body, vec3 position){
    float shadow = getStarTerrainShadowAtPointNoClouds(body, position);
    float highClouds = clamp(celestialGetCloudsRaycast(body, position).r * 1.0, 0.0, 1.0);
    vec3 dirToStar = normalize(ClosestStarPosition - position);
    vec3 color = max(vec3(0.0), getSunColorForRay(body, Ray(position, dirToStar)));
    //if(distance(vec3(0.0), body.position) < body.atmosphereRadius) color *= 0.5;
    return vec4(shadow * color * 0.5, highClouds);
}
CelestialRenderResult renderAtmosphere(RenderPass pass){
    float centerDistance = distance(pass.ray.o, pass.body.position);
    float radius = pass.body.radius;// - pass.body.terrainMaxLevel;
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
            result = renderAtmospherePath(pass, pass.ray.o, pass.atmosphereFarHitPos, 1.0, true);
            vec4 hclouds = pass.isHighCloudsHit ? getHighClouds(pass.body, pass.highCloudsHitPos) : vec4(0.0);
            result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
        }
        else if(hitcount == 2){
            result = renderAtmospherePath(pass, pass.ray.o, planetHit, 1.0, true);
        }
    } else {
        if(hitcount == 1){
            result = renderAtmospherePath(pass, pass.atmosphereNearHitPos, pass.atmosphereFarHitPos, 1.0, true);
            vec4 hclouds = pass.isHighCloudsHit ? getHighClouds(pass.body, pass.highCloudsHitPos) : vec4(0.0);
            result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
        }
        else if(hitcount == 2){
            result = renderAtmospherePath(pass, pass.atmosphereNearHitPos, planetHit, 1.0, true);
            vec4 hclouds = pass.isHighCloudsHit ? getHighClouds(pass.body, pass.highCloudsHitPos) : vec4(0.0);
            result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
            //result.alphaBlendedLight.a = 0.0;//max(hclouds.a, result.alphaBlendedLight.a);
        }
    }
    return result;
}

float fresnelCoefficent(vec3 surfaceDir, vec3 incomingDir, float baseReflectivity){
    return (baseReflectivity + (1.0 - baseReflectivity) * (pow(1.0 - max(0.0, dot(surfaceDir, -incomingDir)), 5.0)));
}

vec3 renderWater(RenderPass pass, vec3 background, float depth){
    vec3 dirToStar = normalize(ClosestStarPosition - pass.waterHitPos);
    vec3 flatnormal = normalize(pass.waterHitPos - pass.body.position);
    float flatdt = max(-0.1, dot(flatnormal, dirToStar));
    vec3 waternormal = celestialGetWaterNormalRaycast(pass.body,  0.000088 * sqrt(pass.waterHit), pass.waterHitPos);

    waternormal = normalize(waternormal);
    float flatdt2 = max(0.0, dot(flatnormal, dirToStar));
    float roughness = mix(0.0, 1.0, clamp(1.0 - 1.0 / (1.0 + pass.waterHit * 50.0), 0.0, 1.0));
    float colormultiplier = 1.0 - roughness * roughness * 0.94;
    float phongMult = mix(442.0, 8.0, roughness);
    waternormal = mix(waternormal, flatnormal, roughness);
    float fresnel = fresnelCoefficent(waternormal, pass.ray.d, 0.04);
    vec3 reflected = normalize(reflect(pass.ray.d, waternormal));
    vec3 reflectedAtmo = getAtmosphereLightForRay(pass, Ray(pass.waterHitPos, reflected), 1.0).additionLight.xyz;
    float refldt = max(0.0, dot(reflected, dirToStar));
    //vec3 result = fresnel * colormultiplier * 10.0 * vec3(0.0, 0.002, 0.006) * max(0.0, flatdt) + reflectedAtmo;
    vec3 result = fresnel * reflectedAtmo;// * (getStarTerrainShadowAtPoint(pass.body, pass.waterHitPos) * 0.7 + 0.3);
    result += fresnel * colormultiplier * reflectedAtmo * pow(refldt, phongMult) * 10.0 * getStarTerrainShadowAtPoint(pass.body, pass.waterHitPos, 0.001);
    result += (1.0 - fresnel) * (background / (depth*depth * 100000.0 + 1.0)) / (depth*depth * 100000.0 + 1.0);
    return scatterLight(pass.body, pass.ray.o, pass.waterHitPos, result);
}

CelestialRenderResult renderCelestialBodyLightAtmosphere(RenderPass pass){
    vec2 tempuv = gl_FragCoord.xy / Resolution;
    vec3 color = texture(surfaceRenderedAlbedoRoughnessImage, tempuv).rgb;
    vec3 normal = normalize(texture(surfaceRenderedNormalMetalnessImage, tempuv).rgb);//celestialGetNormalRaycast(pass.body, sqrt(sqrt(pass.surfaceHit + 1.0)) * 0.004, pass.surfaceHitPos);
    vec3 flatnormal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 ambienceMultiplier = pow(max(0.0, dot(flatnormal, normal)) * 0.9 + 0.1, 12.0) *  ClosestStarColor * 0.0004;
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    color *= getSunColorForRay(pass.body, Ray(pass.surfaceHitPos, dirToStar));
    color *= dt;
    float roughness = 1.0;
    //color *= 2.1;
    color *= getStarTerrainShadowAtPoint(pass.body, pass.surfaceHitPos, 1.0);
    color += getAtmosphereLightForRay(pass, Ray(pass.surfaceHitPos, normal), 0.0).additionLight.xyz * 1.0;
    color = scatterLight(pass.body, pass.ray.o, pass.surfaceHitPos, color);
    float flatdt = max(-0.1, dot(flatnormal, dirToStar));
    //dt = max(dt * smoothstep(-0.1, 0.0, flatdt), flatdt * 0.5);
    CelestialRenderResult atmo = renderAtmosphere(pass);


    if(pass.isSurfaceHit && pass.isWaterHit){
        vec3 surface = vec3(0.0);
        vec3 realSurfaceDir = normalize(pass.surfaceHitPos - pass.body.position);
        vec3 realWaterDir = normalize(pass.waterHitPos - pass.body.position);
        float heightAtDir = pass.body.radius + celestialGetHeight(pass.body, realSurfaceDir) * pass.body.terrainMaxLevel;
        float waterAtDir = getWaterHeightHiRes(pass.body, realWaterDir);
        vec3 posSurface = pass.body.position + realSurfaceDir * heightAtDir;
        vec3 posWater = pass.body.position + realWaterDir * waterAtDir;
        float realDistanceSurface = distance(pass.ray.o, posSurface);//pass.surfaceHit;//texture(surfaceRenderedDistanceImage, tempuv).r;//distance(pass.ray.o, posSurface);
        float realDistanceWater = distance(pass.ray.o, posWater);//pass.waterHit;//texture(waterRenderedDistanceImage, tempuv).r;
        vec3 shadowpos = vec3(0.0);
        if(realDistanceSurface > realDistanceWater){
            surface = renderWater(pass, color, abs(realDistanceSurface - realDistanceWater));// * getHighCloudsShadowAtPoint(pass.body, pass.waterHitPos);
            shadowpos = pass.waterHitPos;
        } else {
            surface = color;
        }
        //surface = mix(renderWater(pass), color, smoothstep(-0.0001, 0.0001, realDistanceWater - realDistanceSurface));
        //surface = normal * 10000.0;
        atmo.alphaBlendedLight = vec4(mix(surface, atmo.alphaBlendedLight.rgb, atmo.alphaBlendedLight.a), 1.0);
    } else if(pass.isWaterHit) {
        vec3 surface = renderWater(pass, vec3(0.0), 100.0);// * getHighCloudsShadowAtPoint(pass.body, pass.waterHitPos);
        atmo.alphaBlendedLight = vec4(mix(surface, atmo.alphaBlendedLight.rgb, atmo.alphaBlendedLight.a), 1.0);
    } else if(pass.isSurfaceHit) {
        vec3 surface = color;
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
