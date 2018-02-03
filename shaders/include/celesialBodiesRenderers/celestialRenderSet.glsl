#pragma once

layout(set = 0, binding = 0) uniform CelestialStorageBuffer {
    vec4 time;
    CelestialBodyAlignedData celestialBody;
} celestialBuffer;

layout(set = 0, binding = 1) uniform sampler2D heightMapImage;
layout(set = 0, binding = 2) uniform sampler2D baseColorImage;
layout(set = 0, binding = 3) uniform sampler2D cloudsImage;
