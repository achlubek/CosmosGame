#pragma once
#include <random>
#include <limits>
#include <glm\glm.hpp>
#include "GeneratedStarInfo.h"
#include "GeneratedPlanetInfo.h"
#include "GeneratedMoonInfo.h"
 

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

