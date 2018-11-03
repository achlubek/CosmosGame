#pragma once

layout(set = 1, binding = 0) uniform CelestialStorageBuffer {
    vec4 time_dataresolution;
    vec4 shadowmapresolution_zero_zero;
    CelestialBodyAlignedData celestialBody;
} celestialBuffer;

layout(set = 1, binding = 1) uniform sampler2D heightMapImage;
layout(set = 1, binding = 2) uniform sampler2D baseColorImage;
layout(set = 1, binding = 3) uniform sampler2D cloudsImage;
layout(set = 1, binding = 4) uniform sampler2D shadowMapImage;
layout(set = 1, binding = 5) uniform sampler2D surfaceRenderedAlbedoRoughnessImage;
layout(set = 1, binding = 6) uniform sampler2D surfaceRenderedNormalMetalnessImage;
layout(set = 1, binding = 7) uniform sampler2D surfaceRenderedEmissionImage;
layout(set = 1, binding = 8) uniform sampler2D surfaceRenderedDistanceImage;
layout(set = 1, binding = 9) uniform sampler2D waterRenderedNormalMetalnessImage;
layout(set = 1, binding = 10) uniform sampler2D waterRenderedDistanceImage;
