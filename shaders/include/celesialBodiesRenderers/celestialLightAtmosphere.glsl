#pragma once

vec4 renderCelestialBodyLightAtmosphere(RenderPass pass){
    return vec4(celestialGetColorRoughnessRaycast(pass.body, pass.surfaceHitPos).xyz, 1.0);
}
