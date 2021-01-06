#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in flat uint inInstanceId;
layout(location = 0) out vec4 outColor;

#include rendererDataSet.glsl
#include camera.glsl
#include sphereRaytracing.glsl

struct GeneratedStarInfo {
    vec4 position_radius;
    vec4 color_zero;
};

layout(set = 1, binding = 0) buffer StarsStorageBuffer {
    GeneratedStarInfo stars[];
} starsBuffer;

GeneratedStarInfo currentStar;

vec3 traceStarGlow(Ray ray){
    // calculate fov coefficent to adjust star size
    float fovCoefficent = sqrt(length(FrustumConeBottomLeftToBottomRight)) * 0.5;

    // get star data
    vec3 starPosition = currentStar.position_radius.xyz;
    float starRadius = currentStar.position_radius.a;

    // transform star position into camera space
    starPosition -= CameraPosition;

    // calculate real distance and clamp it to avoid invisible stars
    float realdist = length(starPosition);
    float dist = min(250000.0 / fovCoefficent, realdist);

    // reconstruct ray hit and star center points with clamped distance
    vec3 rayHitPoint = ray.o + ray.d * dist;
    vec3 starCenterPoint = ray.o + normalize(starPosition) * dist;

    // calculate the distance between ray hit and star center
    float hitDistance = distance(starCenterPoint, rayHitPoint);

    // calculate nice circular shape by 1.0 - distance / radius formula, modulated by fov
    float light = max(0.0, 1.0 - hitDistance / (starRadius * fovCoefficent));

    // some fine tuning
    light *= light;

    // hacky way to dim stars that are beyond distance clamping range
    float cst2 = realdist * 0.000001 * fovCoefficent;
    float dim = clamp(1.0 / (1.0 + cst2 * cst2 * 0.06), 0.0001, 1.0);
    dim = pow(dim, 1.2);

    return dim * light * currentStar.color_zero.xyz;
}

void main() {
    currentStar = starsBuffer.stars[inInstanceId];
    vec4 posradius = currentStar.position_radius;
    posradius.xyz -= CameraPosition;
    Ray cameraRay = Ray(CameraPosition, reconstructCameraSpaceDistance(gl_FragCoord.xy / Resolution, 1.0));

    outColor = vec4(traceStarGlow(cameraRay) * (Exposure * 19660.0), 1.0);

}
