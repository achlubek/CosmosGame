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
    return  body.radius * 0.27;
}


vec3 scatterLight(RenderedCelestialBody body, vec3 observer, vec3 point, vec3 light){

    float primaryLength = max(0.0, rsi2(Ray(point, normalize(observer - point)), body.atmosphereSphere).y);
    primaryLength = min(primaryLength, distance(observer, point));
    //return max(light - (primaryLength * body.atmosphereAbsorbColor * body.radius * 89.0), vec3(0.0));// / (1.0 + primaryLength);
    vec3 colorized = light * pow( 1.0 - body.atmosphereAbsorbColor, vec3(3.0));
    float maxradius = pow(body.radius * 0.035, 1.0);
    float maxradius2 = pow(body.radius * 0.35, 1.0);
    float newpower = 0.0;
    primaryLength = pow(primaryLength, 1.0);
    float mixer = 1.0 - exp(-(primaryLength / maxradius));
    float mixer2 = clamp(primaryLength / maxradius2, 0.0, 1.0);
    return mix(light, colorized, mixer) * mix(newpower, 1.0, pow(1.0 - mixer2, 2.0));

}

vec3 getSunColorForRay(RenderedCelestialBody body, Ray ray){
    float primaryLength = max(0.0, rsi2(ray, body.atmosphereSphere).y);
    return scatterLight(body, ray.o, ray.o + ray.d * primaryLength, ClosestStarColor);// / (1.0 + primaryLength);
}

float getHighCloudsRaw(RenderedCelestialBody body, vec3 p){
    float density = clamp(celestialGetCloudsRaycast(body, p).r * 1.0, 0.0, 1.0);
    return density;
}

CelestialRenderResult renderAtmospherePath(RenderPass pass, vec3 start, vec3 end, float mieMultiplier, bool highQuality){
    //vec3 noonColor = (1.0 - pass.body.atmosphereAbsorbColor) * ClosestStarColor * 0.02;
    //vec3 sunsetColor = (pass.body.atmosphereAbsorbColor) * ClosestStarColor;
    float density = pass.body.atmosphereAbsorbStrength;
    float coverage = 0.0;
    vec3 alphacolor = vec3(0.0);
    vec3 color = vec3(0.0);
    const int steps = 7;//int(oct(UV * 100 * Time) * 20.0) + 1;
    float stepsize = 1.0 / steps;
    #ifdef SHADOW_MAP_COMPUTE_STAGE
    vec2 UV = vec2(0.0);
    #endif
    float iter = stepsize * fract(oct(UV * 100.0) + Time * 0.01);
    float radius = pass.body.radius;
    float atmoheight = pass.body.atmosphereHeight;
    vec3 starDir = normalize(ClosestStarPosition - start);
    vec3 direction = normalize(end - start);
    float rayStarDt = dot(starDir, direction);
    float mieCoeff = exp(-3.1415 * 3.0 * (-rayStarDt * 0.5 + 0.5)) * mieMultiplier;
    float rayleightCoeff = exp(-0.1415 * (-rayStarDt * 0.5 + 0.5));//(1.0 / (1.0 + 12.1 * (  1.0 - (rayStarDt ))));
    float distmultiplier = distance(start, end);
    float shadowAccumulator = 0.0;
    /*if(highQuality){
        float stepsizeShadows = 1.0 / 17.0;
        float iterShadows = stepsizeShadows * oct(UV * Time);
        for(int i=0;i<17;i++){
            //Ray secondaryRay = Ray( mix(start, end, iterShadows), starDir);
            //shadowAccumulator += 1.0 - step(0.0, rsi2(secondaryRay, pass.body.waterSphere).x);
            shadowAccumulator += getStarTerrainShadowAtPoint(pass.body, mix(start, end, iterShadows), 1.0);
            iterShadows += stepsizeShadows;
        }
        shadowAccumulator *= stepsizeShadows;
        //    vec3 normal = normalize(start - pass.body.position);
        //    float dt = 1.0 - (1.0 / (1.0 + 3.0 * max(0.0, dot(normal, starDir) * 0.8 + 0.2)));
        //    shadowAccumulator = dt;
    } else {*/
        float stepsizeShadows = 1.0 / 7.0;
        float iterShadows = stepsizeShadows * oct(end);
        for(int i=0;i<7;i++){
            Ray ray = Ray(mix(start, end, iterShadows), normalize(ClosestStarPosition - mix(start, end, iterShadows)));
            float waterSphereShadow = rsi2(ray, pass.body.waterSphere).x;
            //float highCloudsHit = rsi2(ray, pass.body.highCloudsSphere).y;
            float rings = ringsShadow(pass, ray);
            shadowAccumulator += hits(waterSphereShadow) ? 0.0 : (1.0 - rings);
            iterShadows += stepsizeShadows;
        }
        shadowAccumulator *= stepsizeShadows;
    //}

    // raymarch clouds distance field
    vec3 rayEnergy = vec3(ClosestStarColor);
/*
    vec3 cloudsStart = start;
    vec3 cloudsEnd = end;
    const int Csteps = 27;//int(oct(UV * 100 * Time) * 20.0) + 1;
    float Cstepsize = 1.0 / Csteps;
    float cursor = Cstepsize * fract(oct(UV * 100.0) + Time * 0.01);
    float averagestep = distance(cloudsStart, cloudsEnd) * Cstepsize;
    for(int i=0;i<Csteps;i++){

        vec3 pos = mix(cloudsStart, cloudsEnd, cursor);
        float cdst = distance(pos, pass.body.position) - radius;

        float heightmix = pow(clamp(1.0 - cdst / atmoheight, 0.0, 1.0), 2.0);
        float heightmix2 = pow(clamp( cdst / atmoheight, 0.0, 1.0), 2.0);

        vec3 endSecondary = pos + starDir * rsi2(Ray(pos, starDir), pass.body.atmosphereSphere).y;
        vec3 primaryColor = scatterLight(pass.body, pos, endSecondary, rayEnergy);
        float clouds = smoothstep(0.1, 0.12, getHighCloudsRaw(pass.body, pos) * heightmix * heightmix2 * 2.0);
        alphacolor += primaryColor * (1.0 - coverage) * clouds * (0.7 + heightmix2 * 0.3);
        coverage = min(1.0, coverage + clouds * averagestep * 10.1);
        if(coverage == 1.0) break;
        cursor += Cstepsize;
    }*/

    for(int i=0;i<steps;i++){
        vec3 pos = mix(start, end, iter);
        float cdst = distance(pos, pass.body.position) - radius;
        float heightmix = pow(clamp(1.0 - cdst / atmoheight, 0.0, 1.0), 2.0);
        float heightmix2 = pow(clamp( cdst / atmoheight, 0.0, 1.0), 2.0);

        vec3 endSecondary = pos + starDir * rsi2(Ray(pos, starDir), pass.body.atmosphereSphere).y;
        //rayEnergy = getSunColorForRay(pass.body, Ray(pos, starDir));
        vec3 primaryColor = scatterLight(pass.body, pos, endSecondary, rayEnergy);
        vec3 scattered = primaryColor * mieCoeff * 0.2 + (pass.body.atmosphereAbsorbColor * primaryColor) * rayleightCoeff * 1.0;
        vec3 secondaryColor = scatterLight(pass.body, start, pos, scattered);

        //rayEnergy -= max(vec3(0.0), primaryColor * 0.01);
        color += scattered * heightmix * (1.0 - coverage);
        iter += stepsize;
    }
    color *= distmultiplier * stepsize * pow(shadowAccumulator, 2.0);
    color = max(vec3(0.0), color);
    return CelestialRenderResult(vec4(color, 0.0), vec4(alphacolor, coverage));
}

CelestialRenderResult getAtmosphereLightForRay(RenderPass pass, Ray ray, float mieMultiplier){
    float primaryLength = rsi2(ray, pass.body.atmosphereSphere).y;
    return renderAtmospherePath(pass, ray.o, ray.o + ray.d * primaryLength, mieMultiplier, false);
}


vec4 alphaMix(vec4 a, vec4 b){
    return vec4(mix(a.rgb, b.rgb, b.a), min(1.0, a.a + b.a));
}

vec4 getHighClouds(RenderedCelestialBody body, vec3 position){
    float shadow = 1.0;//getStarTerrainShadowAtPointNoClouds(body, position);
    float highClouds = clamp(celestialGetCloudsRaycast(body, position).r * 1.0, 0.0, 1.0);
    vec3 dirToStar = normalize(ClosestStarPosition - position);
    vec3 color = max(vec3(0.0), getSunColorForRay(body, Ray(position, dirToStar)));
    //if(distance(vec3(0.0), body.position) < body.atmosphereRadius) color *= 0.5;
    return vec4(shadow * color * 1.0, highClouds * 1.0);
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
    vec3 start = vec3(0.0);
    vec3 end = vec3(0.0);
    if(centerDistance < atmoradius){
        if(hitcount == 1){
            start = pass.ray.o;
            end = pass.atmosphereFarHitPos;
            //vec4 hclouds = pass.isHighCloudsHit ? getHighClouds(pass.body, pass.highCloudsHitPos) : vec4(0.0);
            //result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
        }
        else if(hitcount == 2){
            start = pass.ray.o;
            end = planetHit;
            //vec4 hclouds = pass.isHighCloudsHit && pass.highCloudsHit < pass.surfaceHit && pass.highCloudsHit < pass.waterHit ? getHighClouds(pass.body, pass.highCloudsHitPos) : vec4(0.0);
            //result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
        }
    } else {
        if(hitcount == 1){
            start = pass.atmosphereNearHitPos;
            end = pass.atmosphereFarHitPos;
            //vec4 hclouds = pass.isHighCloudsHit ? getHighClouds(pass.body, pass.highCloudsHitPos) : vec4(0.0);
            //result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
        }
        else if(hitcount == 2){
            start = pass.atmosphereNearHitPos;
            end = planetHit;
            //vec4 hclouds = pass.isHighCloudsHit ? getHighClouds(pass.body, pass.highCloudsHitPos) : vec4(0.0);
            //result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
            //result.alphaBlendedLight.a = 0.0;//max(hclouds.a, result.alphaBlendedLight.a);
        }
    }
    result = renderAtmospherePath(pass, start, end, 1.0, true);
    //vec4 hclouds = pass.isHighCloudsHit && pass.highCloudsHit < pass.surfaceHit && pass.highCloudsHit < pass.waterHit ? getHighClouds(pass.body, pass.highCloudsHitPos) : vec4(0.0);
    //result.alphaBlendedLight.rgba = alphaMix(result.alphaBlendedLight.rgba, hclouds);
    return result;
}

float DistributionGGX(vec3 N, vec3 L, float a)
{
    vec3 H = (N + L) * 0.5;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = 3.1415 * denom * denom;

    return nom / denom;
}
vec3 renderWater(RenderPass pass, vec3 background, float depth){
    vec3 dirToStar = normalize(ClosestStarPosition - pass.waterHitPos);
    vec3 flatnormal = normalize(pass.waterHitPos - pass.body.position);
    float flatdt = smoothstep(-0.1, 0.0, max(-0.1, dot(flatnormal, dirToStar)));
    vec3 waternormal = celestialGetWaterNormalRaycast(pass.body,  0.00001288 * sqrt(pass.waterHit), pass.waterHitPos + vec3((sin(Time * 10.0) * 0.5 + 0.5) * 0.0000288 * sqrt(pass.waterHit)));
    float dtup = max(-0.1, dot(waternormal, flatnormal));

    float shadowmult = 1.0 - ringsShadow(pass, Ray(pass.waterHitPos, dirToStar));
    waternormal = normalize(waternormal);
    float flatdt2 = max(0.0, dot(flatnormal, dirToStar));
    float roughness = mix(0.0, 1.0, clamp(1.0 - 1.0 / (1.0 + (pass.waterHit * pass.waterHit ) * 2.38) , 0.0, 1.0));
    float colormultiplier = 1.0 - roughness * roughness * 0.984;
    float phongMult = mix(555.0, 14.0, roughness );
    roughness = roughness * 0.97 + sqrt(roughness) * 0.03 * FBM3(flatnormal * 20.0 + 2.0 * FBM3(flatnormal * 22.0, 5, 1.9, 0.6), 4, 2.0, 0.5);
    waternormal = normalize(mix(waternormal, flatnormal, roughness ));
    float fresnel = fresnelCoefficent(waternormal, pass.ray.d, 0.04);
    vec3 reflected = normalize(reflect(pass.ray.d, waternormal));
    //reflected = normalize(mix(reflected, waternormal, roughness * roughness ));
    vec3 reflectedAtmo = getAtmosphereLightForRay(pass, Ray(pass.waterHitPos, reflected), 1.0).additionLight.xyz;
    float refldt = max(0.0, dot(reflected, dirToStar));
    //vec3 result = fresnel * colormultiplier * 10.0 * vec3(0.0, 0.002, 0.006) * max(0.0, flatdt) + reflectedAtmo;
    float distr = DistributionGGX(waternormal, -dirToStar, 1.0 - roughness * 0.94);
    vec3 result = fresnel * reflectedAtmo;//fresnel * reflectedAtmo;// * (getStarTerrainShadowAtPoint(pass.body, pass.waterHitPos) * 0.7 + 0.3);
    result += flatdt * 10000.0 * distr * fresnel * getSunColorForRay(pass.body, Ray(pass.waterHitPos, reflected)) * pow(refldt, phongMult) * shadowmult;
    //result *= getStarTerrainShadowAtPoint(pass.body, pass.waterHitPos, 0.001);
    result += (1.0 - fresnel) * (background / (depth*depth * 60000.0 + 1.0)) / (depth*depth * 60000.0 + 1.0);
    //result += getAtmosphereLightForRay(pass, Ray(pass.surfaceHitPos, waternormal), 0.0).additionLight.xyz * dtup * 1.0;
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
    color *= getStarTerrainShadowAtPoint(pass.body, pass.surfaceHitPos, 1.0);
    color *= 1.0 - ringsShadow(pass, Ray(pass.surfaceHitPos, dirToStar));
    color += getAtmosphereLightForRay(pass, Ray(pass.surfaceHitPos, normal), 0.0).additionLight.xyz * 1.0;
    color = scatterLight(pass.body, pass.ray.o, pass.surfaceHitPos, color);
    float flatdt = max(-0.1, dot(flatnormal, dirToStar));
    //dt = max(dt * smoothstep(-0.1, 0.0, flatdt), flatdt * 0.5);
    CelestialRenderResult atmo = renderAtmosphere(pass);

    vec3 emission = texture(surfaceRenderedEmissionImage, tempuv).rgb;

    if(pass.isSurfaceHit && pass.isWaterHit){
        vec3 surface = vec3(0.0);
        vec3 realSurfaceDir = normalize(pass.surfaceHitPos - pass.body.position);
        vec3 realWaterDir = normalize(pass.waterHitPos - pass.body.position);
        float heightAtDir = pass.body.radius + celestialGetHeight(pass.body, realSurfaceDir) * pass.body.terrainMaxLevel;
        float waterAtDir = getWaterHeightHiRes(pass.body, realWaterDir);
        vec3 posSurface = pass.body.position + realSurfaceDir * heightAtDir;
        vec3 posWater = pass.body.position + realWaterDir * waterAtDir;
        float realDistanceSurface = texture(surfaceRenderedDistanceImage, tempuv).r;//distance(pass.ray.o, posSurface);
        float realDistanceWater = distance(pass.ray.o, posWater);//pass.waterHit;//texture(waterRenderedDistanceImage, tempuv).r;
        //float realDistanceSurface = texture(surfaceRenderedDistanceImage, tempuv).r;//distance(pass.ray.o, posSurface);
        //float realDistanceWater = texture(waterRenderedDistanceImage, tempuv).r;
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
    //atmo.alphaBlendedLight.rgb += emission;
//    atmo.alphaBlendedLight = rings;
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
