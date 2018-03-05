#pragma once

CelestialRenderResult renderCelestialBodyNoAtmosphere(RenderPass pass){
    vec3 color = celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz;
    float height = celestialGetHeightRaycast(pass.body, pass.surfaceHitPos);
    vec3 normal = celestialGetNormalRaycast(pass.body, sqrt(sqrt(pass.surfaceHit)) * 0.01, pass.surfaceHitPos);
    vec3 flatnormal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    float flatdt = max(0.0, dot(flatnormal, dirToStar));
    dt = max(dt * flatdt, flatdt);
    if(pass.isSurfaceHit){
        return CelestialRenderResult(vec4(0.0),vec4(color * dt * ClosestStarColor, 1.0));
    }
    return CelestialRenderResult(vec4(0.0), vec4(0.0));
}
