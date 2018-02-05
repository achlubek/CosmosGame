#pragma once

vec4 renderCelestialBodyLightAtmosphere(RenderPass pass){
    vec3 color = celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz;
    float height = celestialGetHeightRaycast(pass.body, pass.surfaceHitPos);
    vec3 normal = celestialGetNormalRaycast(pass.body, sqrt(sqrt(pass.surfaceHit + 1.0)) * 0.04, pass.surfaceHitPos);
    vec3 flatnormal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    float flatdt = max(-0.1, dot(flatnormal, dirToStar));
    dt = max(dt * smoothstep(-0.1, 0.0, flatdt), flatdt * 0.5);
    if(pass.isSurfaceHit){
        return vec4(color * dt, 1.0);
    }
    return vec4(0.0);
}
