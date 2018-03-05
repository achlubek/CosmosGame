#pragma once

CelestialRenderResult renderCelestialBodyThickAtmosphere(RenderPass pass){
    vec3 color = celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz;
    vec3 normal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    if(pass.isSurfaceHit){
        return CelestialRenderResult(vec4(0.0),vec4(color * dt * ClosestStarColor, 1.0));
    }
    return CelestialRenderResult(vec4(0.0), vec4(0.0));
}
