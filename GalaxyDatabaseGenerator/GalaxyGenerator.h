#pragma once
#include <random>
#include <limits>
#include <glm\glm.hpp>

struct GeneratedPlanetInfo;
struct GeneratedStarInfo;
struct GeneratedMoonInfo;

struct GeneratedStarInfo {
public:
    int64_t x;
    int64_t y;
    int64_t z;
    uint64_t seed;
    double radius; // real units like 1 391 000
    glm::dvec3 color; //0->maybe 10? maybe 100?
    double age; //0->1
    double spotsIntensity; //0->1
    double rotationSpeed; // 0-> 1
    glm::dvec3 orbitPlane; // normalized direction, normalize(cross(orbitPlane, up_vector)) == planet line
    uint8_t planetsCount;
    uint64_t starIndex;
};

struct GeneratedPlanetInfo {
public:
    GeneratedPlanetInfo(GeneratedStarInfo star) : host(star) {
        
    }
    GeneratedPlanetInfo() { }
    double radius; // real units 12 742
    double terrainMaxLevel; //0->1
    double fluidMaxLevel; //0->1
    double starDistance; // real units like sun to earth 149 600 000
    double habitableChance; //0->1
    double orbitSpeed; //0->1
    glm::vec3 preferredColor;//0->1
    double atmosphereRadius;
    double atmosphereAbsorbStrength;//0->1
    glm::vec3 atmosphereAbsorbColor;//0->1
    uint8_t moonsCount;
    GeneratedStarInfo host;
    uint64_t planetIndex;

};

struct GeneratedMoonInfo {
public:
    GeneratedMoonInfo(GeneratedPlanetInfo planet) : host(planet) {

    }
    GeneratedMoonInfo() { }
    double radius; // real units idk
    double terrainMaxLevel; //0->1
    double fluidMaxLevel; //0->1
    double planetDistance; // real units like sun to earth 149 600 000
    double habitableChance; //0->1
    double orbitSpeed; //0->1
    glm::vec3 preferredColor;//0->1
    double atmosphereRadius;
    double atmosphereAbsorbStrength;//0->1
    glm::vec3 atmosphereAbsorbColor;//0->1
    glm::dvec3 orbitPlane;
    GeneratedPlanetInfo host;
    uint64_t moonIndex;
    uint64_t planetIndex;

};

struct GeneratedStarSystemInfo {
    GeneratedStarInfo star;
    std::vector<GeneratedPlanetInfo> planets;
    std::vector<GeneratedMoonInfo> moons;

};

class GalaxyGenerator
{
public:
    GalaxyGenerator();
    ~GalaxyGenerator();
    std::random_device rd;
    std::mt19937_64 eng;
    uint64_t randu64(uint64_t min, uint64_t max);
    int64_t randi64(int64_t min, int64_t max);
    double drandnorm();
    GeneratedStarSystemInfo generateStar(int64_t galaxyradius, int64_t centerThickness, double centerGravity, uint64_t seed); 
};

