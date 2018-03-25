#pragma once

layout(set = 1, binding = 0) uniform CelestialStorageBuffer {
    vec4 time_dataresolution;
    vec4 shadowmapresolution_zero_zero;
    CelestialBodyAlignedData celestialBody;
} celestialBuffer;

layout(set = 1, binding = 1) uniform sampler2D heightMapImage;
layout(set = 1, binding = 2) uniform sampler2D baseColorImage;
