#pragma once

layout(set = 0, binding = 0) uniform CelestialStorageBuffer {
    vec4 time_dataresolution;
    vec4 shadowmapresolution_zero_zero;
    CelestialBodyAlignedData celestialBody;
} celestialBuffer;

layout(set = 0, binding = 1, r32f) uniform writeonly image2D heightMapImage;
layout(set = 0, binding = 2, rgba16f) uniform writeonly image2D baseColorImage;
layout(set = 0, binding = 3, rg8_snorm) uniform writeonly image2D cloudsImage;
