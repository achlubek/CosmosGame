#pragma once
#include "GeneratedStarInfo.h"
#include "GeneratedPlanetInfo.h"
#include "GeneratedMoonInfo.h"
class GalaxyContainer
{
public:
    GalaxyContainer();
    ~GalaxyContainer();
    std::vector<GeneratedStarInfo>& getAllStars();
    GeneratedStarInfo getClosestPlanet();
private:
    std::vector<GeneratedStarInfo> allStars;
    GeneratedStarInfo closestStar;
    std::vector<GeneratedPlanetInfo> closestStarPlanets;
    std::vector<GeneratedMoonInfo> closestPlanetMoons;

};

