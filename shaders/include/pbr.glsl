#pragma once

float fresnelCoefficent(vec3 surfaceDir, vec3 incomingDir, float baseReflectivity){
    return (baseReflectivity + (1.0 - baseReflectivity) * (pow(1.0 - max(0.0, dot(surfaceDir, -incomingDir)), 5.0)));
}

vec3 fresnel_effect(vec3 base, float roughness, float dt){
    return base + (1.0 - base) * exp(-dt * 6.0 * (1.0 - roughness)) * (1.0 - roughness * 0.9);
}

float G1V(float dotNV, float k)
{
    return 1.0/(dotNV*(1.0-k)+k);
}

vec3 LightingFuncGGX_REF(vec3 N, vec3 V, vec3 L, float roughness, vec3 F0)
{
    float alpha = roughness*roughness;

    vec3 H = normalize(V+L);

    float dotNL = max(0.0, dot(N,L));
    float dotNV = max(0.0, dot(N,V));
    float dotNH = max(0.0, dot(N,H));
    float dotLH = max(0.0, dot(L,H));

    vec3 F;
    float D, vis;

    // D
    float alphaSqr = alpha*alpha;
    float pi = 3.14159;
    float denom = dotNH * dotNH *(alphaSqr-1.0) + 1.0;
    D = alphaSqr/(pi * denom * denom );


    float k = alpha/2.0;
    vis = G1V(dotNL,k)*G1V(dotNV,k);

    vec3 specular = dotNL * F0 * D * vis;
    return specular;
}

vec3 shade_ray(vec3 albedo, vec3 normal, vec3 viewdir, float roughness, float metalness, vec3 lightdir, vec3 lightcolor){

    float dotNL = max(0.0, dot(normal,lightdir));
    vec3 refl = normalize(reflect(viewdir, normal));
    float dotRefL = max(0.0, dot(normalize(mix(refl, normal, roughness * roughness)), lightdir));
    float dotNV = max(0.0, dot(normal,-viewdir));
    float invdotNV = max(0.0, dot(-lightdir,viewdir));
    vec3 spec_color_nonmetal = (1.0 - roughness) * lightcolor * fresnel_effect(vec3(0.04), roughness * 0.99 + 0.01, dotNV) * LightingFuncGGX_REF(normalize(normal), -viewdir, lightdir, roughness * 0.96 + 0.04, vec3(1.0));
    vec3 spec_color_metal = lightcolor * fresnel_effect(albedo, roughness * 0.99 + 0.01, dotNV) * LightingFuncGGX_REF(normalize(normal), -viewdir, lightdir, roughness * 0.98 + 0.02, albedo);
    vec3 diffuse_color_nonmetal = albedo * lightcolor * LightingFuncGGX_REF(normal, -viewdir, lightdir, 1.0, albedo) ;
    return mix(spec_color_nonmetal + diffuse_color_nonmetal, spec_color_metal, metalness);
}
