#pragma once

layout(set = 0, binding = 0) uniform UniformBufferObject1 {
    float Time;
    float Zero;
    vec2 Mouse;
    mat4 VPMatrix;
    vec4 inCameraPos;
    vec4 inFrustumConeLeftBottom;
    vec4 inFrustumConeBottomLeftToBottomRight;
    vec4 inFrustumConeBottomLeftToTopLeft;
    vec4 Resolution;
    vec4 ClosestStarPosition;
    vec4 ClosestStarColor;
} hiFreq;

float Time = hiFreq.Time;

vec3 CameraPosition = hiFreq.inCameraPos.xyz;
vec3 FrustumConeLeftBottom = hiFreq.inFrustumConeLeftBottom.xyz;
vec3 FrustumConeBottomLeftToBottomRight = hiFreq.inFrustumConeBottomLeftToBottomRight.xyz;
vec3 FrustumConeBottomLeftToTopLeft = hiFreq.inFrustumConeBottomLeftToTopLeft.xyz;
vec3 ClosestStarPosition = hiFreq.ClosestStarPosition.xyz;
vec3 ClosestStarColor = hiFreq.ClosestStarColor.xyz;
vec2 Resolution = hiFreq.Resolution.xy;
