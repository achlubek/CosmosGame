#version 450
#extension GL_ARB_separate_shader_objects : enable

#include proceduralValueNoise.glsl

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;
layout(set = 0, binding = 4) uniform sampler2D planetTerrainHeightImage;
layout(set = 0, binding = 5) uniform sampler2D planetTerrainColorImage;
layout(set = 0, binding = 6) uniform sampler2D planetAtmosphereFlunctuationsImage;

#include celestialSet.glsl
#include camera.glsl
#include sphereRaytracing.glsl
#include polar.glsl
#include transparencyLinkedList.glsl
#include rotmat3d.glsl

float rand2s(vec2 co){
    return fract(sin(dot(co.xy * Time,vec2(12.9898,78.233))) * 43758.5453);
}

Ray cameraRay;

GeneratedMoonInfo currentMoon;
GeneratedStarInfo moonHostStar;
GeneratedPlanetInfo moonHostPlanet;

/*
There are only 2 main types of celstial bodies:
    planets and moons
but there are several types of celestial bodies in 2 categories:
1 - Have atmosphere
    a.  Bodies with not dense atmosphere
        thinking here of bodies varying from pluto (very low atmo) by earth to venus
        Means - you can land, you can have fluid oceans, and theres atmosphere layer that you can see through (or not as in venus)
    b. Bodies with very dense atmosphere that is not transparent at all and landing is not possible too (jupiter, saturn, uran, neptune)
        No landing possible, death once crossing atmosphere

2- Don't have atmosphere
    - Rocky stuff that are easily rendered
    Important - No water possible on nonatmospheric bodies!!! easy on rendering, really

These types allow to categorize celestial bodies into following rendering categories:
1. No atmosphere - render only surface (atmosphere height = 0)
2. Little atmosphere - render atmosphere and surface - hardest (atmosphere height > 0  && atmosphere height < 50)
3. Dense atmosphere - render only upper atmosphere layer
*/

#define MSUNDIR (moonHostStar.position_radius.rgb - CameraPosition)
#define MCANDIR (currentMoon.position_radius.rgb)
float getmoonheight(vec3 dir){
    return currentMoon.position_radius.a
        + (1.0 - 2.0 * abs(0.5 - FBM3(dir * 3.0 + 0.3 * FBM3(dir * 4.0, 3, 2.2, 0.5), 7, 2.2, 0.5)))
        * currentMoon.preferredColor_terrainMaxLevel.a;
}
vec3 getmoonnormal(vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    float mult = sqrt(length(MCANDIR));
    mat3 normrotmat1 = rotationMatrix(tangdir, 0.007 * mult);
    mat3 normrotmat2 = rotationMatrix(bitangdir, 0.007 * mult);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * getmoonheight(dir);
    vec3 p2 = dir2 * getmoonheight(dir2);
    vec3 p3 = dir3 * getmoonheight(dir3);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}
vec4 traceMoon(Ray ray){
    Sphere surface = Sphere(currentMoon.position_radius.rgb, currentMoon.position_radius.a);
    vec3 color = vec3(0.0);
    float coverage = 0.0;
    float hit_Surface = rsi2(ray, surface).x;

    if(!hits(hit_Surface) ) return vec4(0.0, 0.0, 0.0, -1.0);
    vec3 norm = getmoonnormal(normalize((ray.d * hit_Surface) - currentMoon.position_radius.rgb));
    float plhei = getmoonheight(normalize((ray.d * hit_Surface) - currentMoon.position_radius.rgb));
    float plheidelta = (plhei - currentMoon.position_radius.a) / currentMoon.preferredColor_terrainMaxLevel.a;
    vec3 newdir = plhei * normalize((ray.d * hit_Surface) - currentMoon.position_radius.rgb);
    vec3 sundir = normalize(MSUNDIR - ray.d * hit_Surface);
    float shadow = smoothstep(-0.2 * plheidelta, 0.2, dot(sundir, newdir));
    color = shadow * vec3(currentMoon.preferredColor_terrainMaxLevel.rgb) * max(0.0, dot(norm, normalize(MSUNDIR - (ray.d * hit_Surface))));
    coverage = 1.0;

    return vec4(color, coverage);
}
// BEGIN PLANET RENDERING

GeneratedPlanetInfo currentPlanet;
GeneratedStarInfo planetHostStar;
#define SUNDIR (planetHostStar.position_radius.rgb - CameraPosition)
vec3 extra_cheap_atmosphere(float raylen, float sunraylen, float absorbstrength, vec3 absorbcolor, float sunraydot){
    //sundir.y = max(sundir.y, -0.07);
    sunraydot = max(0.0, sunraydot);
    raylen *= absorbstrength * 0.004;
    sunraylen *= absorbstrength * 0.004;
    sunraylen = min(sunraylen, 1.8);
    float raysundt = pow(abs(sunraydot), 2.0);
    float sundt = pow(max(0.0, sunraydot), 32.0);
    float mymie = sundt * raylen;
    vec3 suncolor = mix(vec3(1.0), max(vec3(0.0), vec3(1.0) - absorbcolor), clamp(sunraylen, 0.0, 1.0)) / (1.0 + raylen);
    vec3 bluesky= absorbcolor * suncolor;
    vec3 bluesky2 = max(vec3(0.0), bluesky - absorbcolor * 0.08 * (raylen));
    bluesky2 *= raylen * (0.24 + raysundt * 0.24);
    return bluesky2 + mymie * suncolor;
}
#line 231

float clouds(vec3 mx, float h){
    return smoothstep(0.45, 0.75,texture(planetAtmosphereFlunctuationsImage, xyzToPolar(mx)).r * h);
}

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ return dot(point - origin, normal) / dot(direction, normal); }

vec4 traceRings(Ray ray, Sphere surface, vec3 sundir){
    float plane = intersectPlane(ray.o, ray.d, currentPlanet.position_radius.xyz, vec3(0.0, 1.0, 0.0));
    vec3 color = vec3(0.0);
    float coverage = 0.0;
    float hit_Surface = rsi2(ray, surface).x;
    vec3 pos = ray.o + ray.d * plane;
    float r3 = hash(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a + 200.0);
    float planetdist = distance(currentPlanet.position_radius.xyz, pos);
    if(plane > 0.0 && planetdist < currentPlanet.position_radius.a*  (9.0 + r3 * 2.0)){
        float r1 = hash(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a);
        float r2 = hash(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a + 100.0);
        float r4 = hash(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a + 400.0);
        float multprimary = smoothstep(currentPlanet.position_radius.a * 1.4, currentPlanet.position_radius.a * (2.0 + r1 * 2.0), planetdist);
        float multsecondary = 1.0 - smoothstep(currentPlanet.position_radius.a *  (3.0 + r2 * 2.0), currentPlanet.position_radius.a*  (4.0 + r3 * 2.0), planetdist);
        float multflunc = smoothstep(0.1, 0.65, noise1d(planetdist * 0.065 * (0.3 + 0.7 * r4) * 5.0 * noise1d(planetdist * 0.015 * (0.3 + 0.7 * r4))));
        vec3 partcolor = vec3( noise1d(planetdist * 0.065 * (0.3 + 0.7 * r4)),  noise1d(1000.0 + planetdist * 0.065 * (0.3 + 0.7 * r4)),  noise1d(2000.0 + planetdist * 0.065 * (0.3 + 0.7 * r4)));
        float hit_Surface2 = hits(rsi2(Ray(pos, sundir), surface).x) ? 0.0 : 1.0;
        color += partcolor * hit_Surface2 * multprimary * multsecondary * currentPlanet.preferredColor_zero.rgb;
        coverage += multprimary * multsecondary * multflunc;
    }
    storeFragment(plane, coverage, color);
    return vec4(color, coverage);
}
#define maxheight (currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.z * 1.0)//*currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r
float getplanetheight(vec3 dir){
    return currentPlanet.position_radius.a + texture(planetTerrainHeightImage, xyzToPolar(dir)).r * maxheight;
}
vec3 getplanetnormal(vec3 dir){
    vec3 tangdir = normalize(cross(dir, vec3(0.0, 1.0, 0.0)));
    vec3 bitangdir = normalize(cross(tangdir, dir));
    mat3 normrotmat1 = rotationMatrix(tangdir, 0.005);
    mat3 normrotmat2 = rotationMatrix(bitangdir, 0.005);
    vec3 dir2 = normrotmat1 * dir;
    vec3 dir3 = normrotmat2 * dir;
    vec3 p1 = dir * getplanetheight(dir);
    vec3 p2 = dir2 * getplanetheight(dir2);
    vec3 p3 = dir3 * getplanetheight(dir3);
    return normalize(cross(normalize(p3 - p1), normalize(p2 - p1)));
}

float raymarchTerrain(Ray ray, vec3 center, float limit){
    float dist = 0.0;
    float maxheight2 = currentPlanet.position_radius.a + maxheight;
    float lastdst = 999999.0;
    for(int i=0;i<700;i++){
        vec3 p = ray.o + ray.d * dist;
        vec3 dir = normalize(p - center);
        float dc = distance(p, center);
        float ds = dc - getplanetheight(dir);
        if(ds < limit) return dist;
        if(dc > maxheight2 && lastdst < dc) return -0.01;
        lastdst = dc;
        dist += ds * 0.3;
    }
    return -0.01;
}
float raymarchTerrainShadow(Ray ray, vec3 center){
    float dist = rand2s(UV) * 0.2 + 0.07;
    float s = 0.0;
    for(int i=0;i<6;i++){
        vec3 p = ray.o + ray.d * currentPlanet.position_radius.a*0.1 * dist * dist;
        vec3 dir = normalize(p - center);
        float ds = distance(p, center) - getplanetheight(dir);
        s += smoothstep(0.0, 0.1 * dist, ds) ;
        dist += 0.6;
    }
    return clamp(s, 0.0, 1.0);
}

vec3 getAtmosphereColorAtPoint(vec3 raydir, vec3 point){

    vec3 sundir = normalize(SUNDIR - point);
    Sphere surface = Sphere(currentPlanet.position_radius.rgb, currentPlanet.position_radius.a);
    Sphere atmosphere = Sphere(currentPlanet.position_radius.rgb,
        currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);
        float hit_Surface = rsi2(Ray(point, sundir), surface).x;
    vec2 hit_Atmosphere = rsi2(Ray(point, sundir), atmosphere);
    float planetshadow = (1.0 - step(0.0, hit_Surface)) * (1.0 / (1.0 + 10.0 * clamp(max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0)));

    vec3 dir = normalize(point - currentPlanet.position_radius.rgb);
    float f2 = pow(texture(planetAtmosphereFlunctuationsImage, xyzToPolar(-dir)).g, 3.0);
    float absorbstrength = currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.a;
    vec3 atmo = max(vec3(0.0), extra_cheap_atmosphere(1.0, clamp(101.0 * max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0), 323.0
            * absorbstrength,
            currentPlanet.atmosphereAbsorbColor_zero.rgb,
            dot(raydir, sundir)));
    return 10.0*atmo * planetshadow;
}

float getAtmosphereDensityAtPoint(vec3 point){
    float lengthstart = currentPlanet.position_radius.a;
    float lengthend = currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b;
    return smoothstep(lengthstart, lengthend, distance(currentPlanet.position_radius.rgb, point));
}

vec4 tracePlanetAtmosphere(vec3 start, vec3 end, float lengthstart, float lengthstop){
    float seed = currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.a;
    float rd1 = hash(seed);
    float rd2 = hash(seed + 200.0);
    float rd3 = hash(seed + 400.0);
    vec4 result = vec4(0.0);
//    float raylen = distance(start, end);
    Sphere surface = Sphere(currentPlanet.position_radius.rgb, currentPlanet.position_radius.a);
    Sphere atmosphere = Sphere(currentPlanet.position_radius.rgb,
        currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b);

    vec3 atm = vec3(0.0);
    float stepdistance = (currentPlanet.position_radius.a / 6.371) * 0.01;
    float stepdistanceinv = 1.0 / stepdistance;
    float iter = (rand2s(UV)*1.0) * stepdistance;
    float coverage = 1.0;
    float coverage2 = 0.0;
    float dstnew = 0.0;
    float dsttrg = distance(start, end);

    //return dsttrg * vec4(1.0);

    vec3 sundir = normalize(SUNDIR - start);
    #line 192
    vec2 hit_Atmosphere = rsi2(Ray(start, sundir), atmosphere);
    vec3 dir = normalize(start - currentPlanet.position_radius.rgb);
    vec3 drr = normalize(end - start);
    float f2 = pow(texture(planetAtmosphereFlunctuationsImage, xyzToPolar(-dir)).g, 3.0);
    float heightmix = smoothstep(lengthstart, lengthstop, distance(currentPlanet.position_radius.rgb, end));
    float flunctuations = smoothstep(0.013, 0.0135 + 0.24 * f2, mix(0.0, texture(planetAtmosphereFlunctuationsImage, xyzToPolar(dir)).g * noise1d(heightmix * 20.0), sqrt(1.0 - heightmix)));
    float absorbstrength = currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.a * (0.1 + 0.9 * flunctuations);
    vec3 atmo_start = max(vec3(0.0), extra_cheap_atmosphere(101.0 * stepdistance, clamp(101.0 * max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0), 323.0
            * absorbstrength,
            currentPlanet.atmosphereAbsorbColor_zero.rgb *  (0.7 + 0.3 * flunctuations),
            dot(drr, sundir)));
    vec3 directsuncolor =  extra_cheap_atmosphere(clamp(101.0 * max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0), clamp(101.0 * max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0), 323.0
            * absorbstrength,
            currentPlanet.atmosphereAbsorbColor_zero.rgb ,
            1.0) * 0.2 + max(dot(dir, sundir), 0.0);
    sundir = normalize(SUNDIR - end);
    hit_Atmosphere = rsi2(Ray(end, sundir), atmosphere);
    vec3 atmo_end = max(vec3(0.0), extra_cheap_atmosphere(101.0 * stepdistance, clamp(101.0 * max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0), 323.0
            * absorbstrength,
            currentPlanet.atmosphereAbsorbColor_zero.rgb *  (0.7 + 0.3 * flunctuations),
            dot(drr, sundir)));

    vec3 colotest = vec3(0.0);
    while(dstnew < dsttrg){
        vec3 p = start + drr * iter;
        dstnew = distance(start, p);
        colotest += getAtmosphereColorAtPoint(drr, p) * stepdistance * 1000.0;
        iter += stepdistance;
    }
    return vec4(colotest, min(1.0, 1.0 - (0.001 / (distance(start, end) * 10.0 * absorbstrength + 0.01))));

    vec3 ssdir = normalize(start - currentPlanet.position_radius.rgb);
    while(dstnew < dsttrg){
        vec3 p = start + drr * iter;
        vec3 atmocolor = atmo_end; //TODO
        dstnew = distance(start, p);
        sundir = normalize(SUNDIR - p);
        hit_Atmosphere = rsi2(Ray(p, sundir), atmosphere);
        float hit_Surface = rsi2(Ray(p, sundir), surface).x;//raymarchTerrain(Ray(p, sundir), currentPlanet.position_radius.xyz, 0.001);
        iter += stepdistance;
        float dst = length(p - currentPlanet.position_radius.rgb);
        vec4 coorddir = vec4(dir * (1.0 + 0.1 * heightmix) * 11.0 * (0.2 + rd2 * 3.0), hiFreq.Time * 0.001);
        float cloudiness = clouds(dir ,  1.0 - abs( heightmix * 2.0 - 1.0)  );
        float shadow = 1.0;// - traceRings(Ray(p, sundir), surface, sundir).a;
        vec3 AC = atmocolor;

        float planetshadow = (1.0 - step(0.0, hit_Surface)) * (1.0 / (1.0 + 10.0 * clamp(max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0)));
        vec3 cloudscolor = mix(currentPlanet.atmosphereAbsorbColor_zero.rgb * 0.3, vec3(1.0), currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.r);
        atm += vec3(cloudscolor * stepdistance * cloudiness * directsuncolor * planetshadow  * clamp(coverage, 0.0, 1.0) * 15000.507 * (0.3+0.7 * ( heightmix)))
            + clamp( heightmix* flunctuations  * clamp(coverage, 0.0, 1.0) * 56990.18 * stepdistance * planetshadow * (AC ) * clamp(1.0 - 0.4 * clamp(max(hit_Atmosphere.x, hit_Atmosphere.y), 0.0, 10000.0) * absorbstrength, 0.0, 1.0), 0.0,100000.0);
        coverage *= 1.0 - (heightmix * stepdistance * absorbstrength * 0.507 *flunctuations  + stepdistance * 5000.507 * cloudiness);
        //coverage2 += heightmix * stepdistance * absorbstrength * 50.507 * flunctuations + cloudiness;
        if(coverage < 0.01) break;
    }
    //atm *= 0.1;
    return vec4(atm, 1.0+clamp(1.0 - coverage, 0.0, 1.0));
}

vec4 tracePlanet(Ray ray){
    Sphere surface = Sphere(currentPlanet.position_radius.rgb, currentPlanet.position_radius.a);
    float atmoradius = currentPlanet.position_radius.a + currentPlanet.habitableChance_orbitSpeed_atmosphereRadius_atmosphereAbsorbStrength.b ;
    Sphere atmosphere = Sphere(currentPlanet.position_radius.rgb,
        atmoradius);
    vec3 color = vec3(0.0);
    float coverage = 0.0;
    vec2 hit_Atmosphere = rsi2(ray, atmosphere);
    float hit_Surface = 0.0;
    float hit_Water = 0.0;
    if(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r == 0.0){
        hit_Surface = rsi2(ray, surface).x;
    } else if(hits(hit_Atmosphere.x) || hits(hit_Atmosphere.y)){
        hit_Surface = raymarchTerrain(ray, currentPlanet.position_radius.xyz, 0.001);
    }

    float centerDistance = distance(ray.o, currentPlanet.position_radius.rgb);

        /* It has multiple scenarios possible to happen
        1. Ray origin is outside of atmosphere
            a: Ray hits nothing - return nothing
            b: Ray hits atmosphere but not the ground - trace atmosphere and return
            c: Ray hits both atmosphere and ground - trace planet then atmosphere, alpha mix and return

        2. Ray origin is in atmosphere but not in the ground
            a: Ray doesnt hit ground - trace atmosphere and return
            b: Ray hits ground - trace planet then atmosphere, alpha mix and return

        3. Ray origin is in the ground
            Ray will ALWAYS hit the ground so basically return black color with full alpha

        */

    int hitcount = 0;
    if(hits(hit_Surface)) hitcount++;
    if(hits(hit_Atmosphere.x) || hits(hit_Atmosphere.y)) hitcount++;
    if(centerDistance >= currentPlanet.position_radius.a && centerDistance >= atmoradius){
        // scenario 1
        if(hitcount == 0) coverage = -1.0;//scenario a
        if(hitcount == 1){ // scenario b
            vec4 atm = tracePlanetAtmosphere(ray.d * hit_Atmosphere.x, ray.d * hit_Atmosphere.y,
                currentPlanet.position_radius.a , atmoradius);
            color = atm.rgb;
            coverage = atm.a;
        }
        if(hitcount == 2){ // scenario c
            vec4 atm = tracePlanetAtmosphere(ray.d * hit_Atmosphere.x, ray.d * hit_Surface,
                currentPlanet.position_radius.a, atmoradius);
            vec3 norm = getplanetnormal(normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb));
            vec3 sdir = normalize(SUNDIR - (ray.d * hit_Surface));
            vec3 dir = normalize(ray.d * hit_Surface - currentPlanet.position_radius.rgb);
            vec4 datatex = texture(planetTerrainColorImage, xyzToPolar(dir)).rgba;
            float hit_Surface = raymarchTerrain(Ray(ray.d * hit_Surface, sdir), currentPlanet.position_radius.xyz, 0.001);
            float shadow = 0.1 + 0.9 * (1.0 - step(0.0, hit_Surface));
            vec3 colo = shadow * 4000.0 *  datatex.rgb * pow(max(0.0, dot(norm, sdir)), 2.0 + 3.0 * (1.0 - datatex.a));
            color = atm.rgb * atm.a + (1.0 - atm.a) * colo;
            coverage = 1.0;
        }
    } else if(centerDistance >= currentPlanet.position_radius.a && centerDistance < atmoradius){
        if(hitcount == 1){ // scenario b
            vec4 atm = tracePlanetAtmosphere(ray.o, ray.d * hit_Atmosphere.y,
                currentPlanet.position_radius.a , atmoradius);
            color = atm.rgb;
            coverage = atm.a;
        }
        if(hitcount == 2){ // scenario c
            vec4 atm = tracePlanetAtmosphere(ray.o, ray.d * hit_Surface,
                currentPlanet.position_radius.a , atmoradius);
            vec3 norm = getplanetnormal(normalize((ray.d * hit_Surface) - currentPlanet.position_radius.rgb));
            vec3 sdir = normalize(SUNDIR - (ray.d * hit_Surface));
            vec3 dir = normalize(ray.d * hit_Surface - currentPlanet.position_radius.rgb);
            vec4 datatex = texture(planetTerrainColorImage, xyzToPolar(dir)).rgba;
            float hit_Surface = raymarchTerrain(Ray(ray.d * hit_Surface, sdir), currentPlanet.position_radius.xyz, 0.001);
            float shadow = 0.1 + 0.9 * (1.0 - step(0.0, hit_Surface));
            vec3 colo = shadow * 4000.0 *  datatex.rgb * pow(max(0.0, dot(norm, sdir)), 1.0 + 3.0 * (1.0 - datatex.a));
            color = atm.rgb * atm.a  + (1.0 - atm.a) * colo;
            coverage = 1.0;
        }
    } else if(centerDistance < currentPlanet.position_radius.a && centerDistance < atmoradius){
        coverage = 1.0; // scenario 3 return black full alpha
    }

    if(currentPlanet.terrainMaxLevel_fluidMaxLevel_starDistance_seed.r <= 0.001){
        vec4 rings = traceRings(ray, surface, normalize(SUNDIR - currentPlanet.position_radius.rgb));
    }

    return vec4(color, min(1.0, coverage));
}
// END PLANET RENDERING

void main() {
    vec3 dir = reconstructCameraSpaceDistance(UV, 1.0);
    Ray cameraRay = Ray(vec3(0.0), dir);
    for(int i=0;i<planetsBuffer.count.x;i++){
        currentPlanet = planetsBuffer.planets[i];
        float dist = length(currentPlanet.position_radius.rgb);
        //if(dist * dist > 99999.0) continue;
        planetHostStar = starsBuffer.stars[currentPlanet.hoststarindex_zero_zero_zero.x];
        vec4 pl = tracePlanet(cameraRay);
        //if(pl.a < -0.9) continue;
        if(pl.a > 0.0) storeFragment(dist, max(pl.a, 0.0), pl.rgb);
    }
    for(int i=0;i<moonsBuffer.count.x;i++){
        currentMoon = moonsBuffer.moons[i];
        moonHostPlanet = planetsBuffer.planets[currentMoon.hostplanetindex_zero_zero_zero.x];
        moonHostStar = starsBuffer.stars[moonHostPlanet.hoststarindex_zero_zero_zero.x];
        float dist = length(currentMoon.position_radius.rgb);
        //if(dist * dist > 99999.0) continue;
        vec4 pl = traceMoon(cameraRay);
        //if(pl.a < -0.9) continue;
        if(pl.a > 0.0) storeFragment(dist, max(pl.a, 0.0), pl.rgb);
    }
    outColor = resolveFragments() - vec4((1.0 / 128.0) * vec3(rand2s(UV)), 0.0);
}
