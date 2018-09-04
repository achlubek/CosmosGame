#pragma once
#include <random>
#include <limits>
#include <glm\glm.hpp>
#include "Star.h"
#include "CelestialBody.h"
 

struct GeneratedStarSystemInfo {
    Star* star;
    std::vector<CelestialBody*> bodies;

};

class GalaxyGenerator
{
public:
    GalaxyGenerator();
    ~GalaxyGenerator();
    std::random_device rd;
    std::mt19937_64 eng;
    uint64_t rand_uint64(uint64_t min, uint64_t max);
    int64_t rand_int64(int64_t min, int64_t max);
    double rand_normalized_double();
    GeneratedStarSystemInfo* generateStar(int64_t galaxyradius, int64_t centerThickness, double centerGravity, uint64_t seed); 
};

