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
    vec4 Resolution_Exposure_Zero;
    mat4 FromStarToThisMatrix;
    vec4 ClosestStarPosition;
    vec4 ClosestStarColor;
    ivec4 ShadowMapCount;
    float ShadowMapDivisors1;
    float ShadowMapDivisors2;
    float ShadowMapDivisors3;
    float ShadowMapDivisors4;
} hiFreq;

float Time = hiFreq.Time;

vec3 CameraPosition = hiFreq.inCameraPos.xyz;
vec3 FrustumConeLeftBottom = hiFreq.inFrustumConeLeftBottom.xyz;
vec3 FrustumConeBottomLeftToBottomRight = hiFreq.inFrustumConeBottomLeftToBottomRight.xyz;
vec3 FrustumConeBottomLeftToTopLeft = hiFreq.inFrustumConeBottomLeftToTopLeft.xyz;
vec3 ClosestStarPosition = hiFreq.ClosestStarPosition.xyz;
vec3 ClosestStarColor = hiFreq.ClosestStarColor.xyz;
vec2 Resolution = hiFreq.Resolution_Exposure_Zero.xy;
float Exposure = hiFreq.Resolution_Exposure_Zero.z;
float ShadowMapCount = hiFreq.ShadowMapCount.z;
float ShadowMapDivisors1 = hiFreq.ShadowMapDivisors1;
float ShadowMapDivisors2 = hiFreq.ShadowMapDivisors2;
float ShadowMapDivisors3 = hiFreq.ShadowMapDivisors3;
float ShadowMapDivisors4 = hiFreq.ShadowMapDivisors4;
mat4 FromStarToThisMatrix = hiFreq.FromStarToThisMatrix;
