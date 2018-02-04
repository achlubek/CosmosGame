#pragma once

layout(set = 0, binding = 0) uniform CelestialStorageBuffer {
    vec4 time_dataresolution;
    CelestialBodyAlignedData celestialBody;
} celestialBuffer;

layout(set = 0, binding = 1, r16f) uniform writeonly image2D heightMapImage;
layout(set = 0, binding = 2, rgba16f) uniform writeonly image2D baseColorImage;
layout(set = 0, binding = 3, rg16f) uniform writeonly image2D cloudsImage;
