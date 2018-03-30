#pragma once

CelestialRenderResult renderCelestialBodyNoAtmosphere(RenderPass pass){
    vec2 tempuv = gl_FragCoord.xy / Resolution;
    vec3 color = texture(surfaceRenderedAlbedoRoughnessImage, tempuv).rgb;
    vec3 normal = normalize(texture(surfaceRenderedNormalMetalnessImage, tempuv).rgb);
    vec3 flatnormal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    float flatdt = max(0.0, dot(flatnormal, dirToStar));
    //dt = dtmax(dt * flatdt, flatdt);
    if(pass.isSurfaceHit){
        return CelestialRenderResult(vec4(0.0),vec4(color * dt * ClosestStarColor, 1.0));
    }
    return CelestialRenderResult(vec4(0.0), vec4(0.0));
}
