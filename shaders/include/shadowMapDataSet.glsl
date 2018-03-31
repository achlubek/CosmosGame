#pragma once

layout(set = 2, binding = 0) uniform UniformBufferObjectShadowMapData {
    vec4 Divisor;
} shadowMapData;

float Divisor = shadowMapData.Divisor.x;
