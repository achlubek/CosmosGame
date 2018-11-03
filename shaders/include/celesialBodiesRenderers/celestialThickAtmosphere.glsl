#pragma once

CelestialRenderResult renderCelestialBodyThickAtmosphere(RenderPass pass){
/*    vec3 color = celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz;
    vec3 normal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    if(pass.isSurfaceHit){
        return CelestialRenderResult(vec4(0.0),vec4(color * dt * ClosestStarColor, 1.0));
    }
    return CelestialRenderResult(vec4(0.0), vec4(0.0));
*/
    vec2 tempuv = gl_FragCoord.xy / Resolution;
    vec3 color = texture(surfaceRenderedAlbedoRoughnessImage, tempuv).rgb;
    vec3 normal = normalize(texture(surfaceRenderedNormalMetalnessImage, tempuv).rgb);
    vec3 emission = texture(surfaceRenderedEmissionImage, tempuv).rgb;
    vec3 flatnormal = normalize(pass.surfaceHitPos - pass.body.position);
    vec3 dirToStar = normalize(ClosestStarPosition - pass.surfaceHitPos);
    float dt = max(0.0, dot(normal, dirToStar));
    float flatdt = max(0.0, dot(flatnormal, dirToStar));
    float fresnel = 0.1;// fresnelCoefficent(normal, pass.ray.d, 0.04);
    //dt = dtmax(dt * flatdt, flatdt);
    if(pass.isSurfaceHit){
        return CelestialRenderResult(vec4(0.0),vec4(color * dt * fresnel * ClosestStarColor + emission, 1.0));
    }
    return CelestialRenderResult(vec4(0.0), vec4(0.0));
}
