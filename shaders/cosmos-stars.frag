#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 1) in flat uint inInstanceId;
layout(location = 2) in vec3 inWorldPos;
layout(location = 0) out vec4 outColor;

#include rendererDataSet.glsl
#include camera.glsl

struct GeneratedStarInfo {
    vec4 position_radius;
    vec4 color_age; //0->maybe 10? maybe 100?
    vec4 orbitPlane_rotationSpeed;
    vec4 spotsIntensity_zero_zero_zero; //0->1
};

layout(set = 1, binding = 0) buffer StarsStorageBuffer {
    ivec4 count;
    GeneratedStarInfo stars[];
} starsBuffer;


struct Ray { vec3 o; vec3 d; };
struct Sphere { vec3 pos; float rad; };
float rsi2_simple(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    return -b - sqrt(b * b - 4.0 * (dot(oc, oc) - sphere.rad*sphere.rad));
}
vec2 rsi2(in Ray ray, in Sphere sphere)
{
    vec3 oc = ray.o - sphere.pos;
    float b = 2.0 * dot(ray.d, oc);
    float c = dot(oc, oc) - sphere.rad*sphere.rad;
    float disc = b * b - 4.0 * c;
    vec2 ex = vec2(-b - sqrt(disc), -b + sqrt(disc))/2.0;
    return vec2(min(ex.x, ex.y), max(ex.x, ex.y));
}
float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}

float infinity = 1.0 / 0.0;
Ray cameraRay;
#define hits(a) (a > 0.0 && a < infinity)

// BEGIN STAR RENDERING

GeneratedStarInfo currentStar;

vec3 traceStarGlow(Ray ray){
    float dtraw = dot(normalize(currentStar.position_radius.rgb  - ray.o), ray.d);
    float dotz = max(0.0, dtraw);


    vec4 posradius = currentStar.position_radius;
    posradius.xyz -= CameraPosition;
    float realdist = length(posradius.xyz);
    float dist = min(500000.0, realdist);
    float camdist = dist;

    vec3 reconpoint = ray.o + ray.d * camdist;
    float deltadst = distance(ray.o + normalize(posradius.xyz) * dist, reconpoint);
    float prcnt = max(0.0, 1.0 - deltadst / (currentStar.position_radius.a * 1.0));
    float prcnt2 = max(0.0, 1.0 - deltadst / (currentStar.position_radius.a * 4.0));
    prcnt *= prcnt;
    prcnt2 *= prcnt2 * prcnt2 * prcnt2 * prcnt2;
    camdist = distance(CameraPosition, currentStar.position_radius.xyz);
    camdist *= 0.001;
    //camdist = min(camdist, 66000.0);
    float cst2 = camdist * 0.001;
    float dim = clamp(1.0 /(1.0 + cst2 * cst2 * 0.06), 0.0001, 1.0);
    dim = pow(dim,1.2);
    float additionalCloseMultiplier = 1.0 + (1.0 - smoothstep(100.0, 100000.0, realdist)) * 10000.0;

    return dim * (smoothstep(0.00, 0.3, prcnt) * 14.9 ) * currentStar.color_age.xyz;

}

void main() {
    currentStar = starsBuffer.stars[inInstanceId];
    vec4 posradius = currentStar.position_radius;
    posradius.xyz -= CameraPosition;
    Ray cameraRay = Ray(CameraPosition, reconstructCameraSpaceDistance(gl_FragCoord.xy / Resolution, 1.0));

    outColor = vec4(traceStarGlow(cameraRay) * (Exposure * 1000.0), 1.0);

}
