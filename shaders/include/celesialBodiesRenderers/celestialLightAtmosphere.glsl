#pragma once


CelestialRenderResult renderAtmospherePath(RenderPass pass, vec3 start, vec3 end){
    vec3 noonColor = 1.0 - pass.body.atmosphereAbsorbColor;
    vec3 sunsetColor = pass.body.atmosphereAbsorbColor;
    float density = pass.body.atmosphereAbsorbStrength;
    float coverage = 0.0;
    vec3 color = vec3(0.0);
    float stepsize = 1.0 / 100.0;
    float iter = 0.0;
    float radius = pass.body.radius;
    float atmoheight = pass.body.atmosphereHeight;
    float mieCoeff = (1.0 / (0.1 + 990.0 * (1.0 - max(0.0, dot(normalize(ClosestStarPosition - pass.ray.o), pass.ray.d)))));
    for(int i=0;i<100;i++){
        vec3 pos = mix(start, end, iter);
        float cdst = distance(pos, pass.body.position) - radius;
        float heightmix = 1.0 - cdst / atmoheight;
        vec3 normal = normalize(pos - pass.body.position);
        vec3 dirToStar = normalize(ClosestStarPosition - pos);
        float dt = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(normal, dirToStar))));
        color += ClosestStarColor * 100.0 * heightmix * distance(start, end) * mix(noonColor, sunsetColor, dt) * dt * (1.0 + mieCoeff * 100.0);
        coverage += distance(start, end) * 0.001;
        iter += stepsize;
    }
    color *= stepsize;
    return CelestialRenderResult(vec4(color, 0.0), vec4(0.0));
}

CelestialRenderResult renderAtmosphere(RenderPass pass){
    float centerDistance = distance(pass.ray.o, pass.body.position);
    float radius = pass.body.radius;
    float atmoradius = pass.body.atmosphereRadius;
    int hitcount = 0;
    if(pass.isSurfaceHit) {
        hitcount++;
    }
    if(pass.isAtmosphereHit) {
        hitcount++;
    }
    if(hitcount == 0){
        return emptyAtmosphereResult;
    }
    if(centerDistance < radius){
        return emptyAtmosphereResult;
    }
    else if(centerDistance < atmoradius){
        if(hitcount == 1){
            return renderAtmospherePath(pass, pass.ray.o, pass.atmosphereFarHitPos);
        }
        else if(hitcount == 2){
            return renderAtmospherePath(pass, pass.ray.o, pass.surfaceHitPos);
        }
    } else {
        if(hitcount == 1){
            return renderAtmospherePath(pass, pass.atmosphereNearHitPos, pass.atmosphereFarHitPos);
        }
        else if(hitcount == 2){
            return renderAtmospherePath(pass, pass.atmosphereNearHitPos, pass.surfaceHitPos);
        }
    }
    return emptyAtmosphereResult;
}

CelestialRenderResult renderCelestialBodyLightAtmosphere(RenderPass pass){
    vec3 color = celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz;
    float height = celestialGetHeightRaycast(pass.body, pass.surfaceHitPos);
    vec3 normal = celestialGetNormalRaycast(pass.body, sqrt(sqrt(pass.surfaceHit + 1.0)) * 0.04, pass.surfaceHitPos);
    vec3 flatnormal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    float flatdt = max(-0.1, dot(flatnormal, dirToStar));
    dt = max(dt * smoothstep(-0.1, 0.0, flatdt), flatdt * 0.5);
    CelestialRenderResult atmo = renderAtmosphere(pass);
    if(pass.isSurfaceHit){
        vec3 surface = color * dt;
        atmo.alphaBlendedLight = vec4(surface, 1.0);
    }

    float threshold = pass.body.radius * 10.0;
    vec4 summedAlpha = vec4(atmo.additionLight.rgb + atmo.alphaBlendedLight.rgb, 1.0);
    vec4 reducedAdditive = vec4(0.0);
    float mixvalue = clamp(distance(pass.ray.o, pass.body.position) / threshold, 0.0, 1.0);
    atmo.additionLight = mix(atmo.additionLight, reducedAdditive, mixvalue);
    atmo.alphaBlendedLight = mix(atmo.alphaBlendedLight, summedAlpha, mixvalue);
    return atmo;
}
