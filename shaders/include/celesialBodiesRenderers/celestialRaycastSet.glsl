#pragma once

layout(set = 0, binding = 1) buffer UniformBufferObject2 {
    ivec4 count;
    vec4 position[];
} raycastInputBuffer;

layout(set = 1, binding = 0) uniform CelestialStorageBuffer {
    vec4 time_dataresolution;
    vec4 shadowmapresolution_zero_zero;
    CelestialBodyAlignedData celestialBody;
} celestialBuffer;

layout(set = 1, binding = 1) uniform sampler2D heightMapImage;
layout(set = 1, binding = 2) uniform sampler2D baseColorImage;
layout(set = 1, binding = 3) uniform sampler2D cloudsImage;

layout(set = 1, binding = 4) buffer raycastResultBufferName {
    vec4 position[];
} raycastResultBuffer;

int RaycastPointsCount = raycastInputBuffer.count.x;
vec3 getRaycastPoint(int i){
    return raycastInputBuffer.position[i].xyz;
}
