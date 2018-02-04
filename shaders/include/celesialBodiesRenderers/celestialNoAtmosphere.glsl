#pragma once

vec4 renderCelestialBodyNoAtmosphere(RenderPass pass){
    vec3 color = celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz;
    float height = celestialGetHeightRaycast(pass.body, pass.surfaceHitPos);
    vec3 normal = celestialGetNormalRaycast(pass.body, pass.surfaceHitPos);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    if(pass.isSurfaceHit){
        return vec4(color * dt, 1.0);
    }
    return vec4(0.0);
}
