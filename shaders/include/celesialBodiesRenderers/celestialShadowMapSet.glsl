#pragma once

layout(set = 1, binding = 0) uniform CelestialStorageBuffer {
    vec4 time_dataresolution;
    vec4 shadowmapresolution_offset_zero;
    CelestialBodyAlignedData celestialBody;
} celestialBuffer;

layout(set = 1, binding = 1) uniform sampler2D heightMapImage;
layout(set = 1, binding = 2) uniform sampler2D baseColorImage;
layout(set = 1, binding = 3) uniform sampler2D cloudsImage;
layout(set = 1, binding = 4, rg16f) uniform coherent image2D shadowMapImage;
