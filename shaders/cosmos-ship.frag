 #version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 1) in flat uint inInstanceId;
layout(location = 2) in vec3 inWorldPos;
layout(location = 3) in vec3 inNormal;
layout(location = 4) in vec4 inTangent;

layout(location = 0) out vec4 outAlbedoRoughness;
layout(location = 1) out vec4 outEmission;
layout(location = 2) out vec4 outNormalMetalness;
layout(location = 3) out float outDistance;
layout(set = 1, binding = 1) uniform sampler2D texAlbedo;
layout(set = 1, binding = 2) uniform sampler2D texNormal;
layout(set = 1, binding = 3) uniform sampler2D texRoughness;
layout(set = 1, binding = 4) uniform sampler2D texMetalness;
layout(set = 1, binding = 5) uniform sampler2D texEmissionIdle;
layout(set = 1, binding = 6) uniform sampler2D texEmissionPowered;

layout(set = 1, binding = 0) buffer modelStorageBuffer {
    mat4 transformation;
    vec4 position;
    ivec4 id;
    vec4 emissionvalue;
} modelBuffer;

#include rendererDataSet.glsl

#include camera.glsl
#include sphereRaytracing.glsl
#include polar.glsl
#include transparencyLinkedList.glsl
#include rotmat3d.glsl

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 closestPointOnLine(vec3 point, vec3 start, vec3 end) {
    vec3 a = point - start;
    vec3 b = end - start;
    float len = distance(a, b);
    return start + clamp(dot(a, b) / dot(b, b), 0.0, 1.0) * b;
}

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{
    return dot(point - origin, normal) / dot(direction, normal);
}

float peaksmoothstep(float peak, float val){
    return smoothstep(0.0, peak, val) * (1.0 - smoothstep(peak, 1.0, val));
}

vec3 thrustengine(vec3 rayorigin, vec3 raydir, vec3 position, vec3 direction, float wide, float strength){
    vec3 closestPoint = closestPointOnLine(rayorigin, position - direction * 100.0, position + direction * 100.0);
    vec3 planenormal = normalize(rayorigin - closestPoint);
    vec3 result = vec3(0.0);
    mat3 rotmat = rotationMatrix(direction, 2.3999);
    for(int i=0;i<10;i++){
        planenormal = rotmat * planenormal;
        float planedst = intersectPlane(rayorigin, raydir, closestPoint, planenormal);
        //if(planedst <= 0.0) return vec3(0.0);
        vec3 point = rayorigin + raydir * planedst;
        float sourcedst = distance(position, point) * 10.0;
        vec3 reldir = normalize(point - position);
        float sourcedt = max(0.0, dot(direction, reldir));
        float att = 1.0 / (1.0 + sourcedst*sourcedst);
        float power = min(0.99, pow(sourcedt, 64.0 / wide) * att * strength);
        result += vec3(1.0, 0.0, 0.0) * peaksmoothstep(0.1, power) / strength;
        result += vec3(0.0, 1.0, 0.0) * peaksmoothstep(0.2, power);
        result += vec3(0.0, 1.0, 1.0) * peaksmoothstep(0.99, power);
        result += vec3(0.0, 0.0, 1.0) * peaksmoothstep(0.1, power);
    }
    return result * 0.1;
}
Ray cameraRay;
void main() {
    vec2 texuv = vec2(UV.x, 1.0 - UV.y);
    vec3 tangent = normalize(inTangent.rgb);
    vec3 normal = normalize(inNormal);

    float tangentSign = inTangent.w;
    mat3 TBN = (mat3(
        tangent,
        normalize(cross(normal, tangent)) * tangentSign,
        normal
    ));

    vec3 normalmaptex = normalize(texture(texNormal, texuv).rgb * 2.0 - 1.0);
    //normalmaptex.r *= -1.0;

    normal = TBN * normalize(normalmaptex);

    outAlbedoRoughness = vec4(texture(texAlbedo, texuv).rgb, texture(texRoughness, texuv).r);
    outNormalMetalness = vec4(normalize(normal),texture(texMetalness, texuv).r);
    outDistance = length(inWorldPos);
    outEmission = vec4(mix(texture(texEmissionIdle, texuv).rgb, texture(texEmissionPowered, texuv).rgb, modelBuffer.emissionvalue.x), 0.0);
    float C = 0.001;
    float w = length(inWorldPos);
    float Far = 10000.0;
    gl_FragDepth = min(1.0, log(C*w + 1.0) / log(C*Far + 1.0));
}
