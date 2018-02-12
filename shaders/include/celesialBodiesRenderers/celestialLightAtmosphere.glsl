#pragma once

struct CelestialRenderResult
{
    vec3 additionLight;
    vec4 alphaBlendedLight;
};

CelestialRenderResult emptyAtmosphereResult = CelestialRenderResult(vec3(0.0), vec4(0.0));

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
    for(int i=0;i<100;i++){
        vec3 pos = mix(start, end, iter);
        float cdst = distance(pos, pass.body.position) - radius;
        float heightmix = 1.0 - cdst / atmoheight;
        vec3 normal = normalize(pos - pass.body.position);
        vec3 dirToStar = normalize(ClosestStarPosition - pos);
        float dt = 1.0 - (1.0 / (1.0 + 10.0 * max(0.0, dot(normal, dirToStar))));
        color += ClosestStarColor * 100.0 * heightmix * distance(start, end) * mix(noonColor, sunsetColor, dt) * dt;
        coverage += distance(start, end) * 0.001;
        iter += stepsize;
    }
    color *= stepsize;
    return CelestialRenderResult(color, vec4(0.0));
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

vec4 renderCelestialBodyLightAtmosphere(RenderPass pass){
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
        return vec4(surface + atmo.additionLight, 1.0);
    } else {
        return vec4(atmo.additionLight * 100.0, 0.01);
    }
}
