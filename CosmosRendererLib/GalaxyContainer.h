#pragma once
#include "GeneratedStarInfo.h"
#include "GeneratedPlanetInfo.h"
#include "GeneratedMoonInfo.h"
#include "SQLiteDatabase.h"
class GalaxyContainer
{
public:
    GalaxyContainer();
    ~GalaxyContainer();
    std::vector<GeneratedStarInfo>& getAllStars();
    GeneratedStarInfo getClosestStar();
    GeneratedPlanetInfo getClosestPlanet();
    GeneratedMoonInfo getClosestMoon();
    std::vector<GeneratedPlanetInfo>& getClosestStarPlanets();
    std::vector<GeneratedMoonInfo>& getClosestPlanetMoons();
    glm::dvec3 getGravity(glm::dvec3 observerPosition);
    long getStarsCount();
    void loadFromDatabase(SQLiteDatabase* db);
    void update(glm::dvec3 observerPosition);
private:
    std::vector<GeneratedStarInfo> allStars;
    std::vector<GeneratedPlanetInfo> allPlanets;
    std::vector<GeneratedMoonInfo> allMoons;
    GeneratedStarInfo closestStar;
    GeneratedPlanetInfo closestPlanet;
    GeneratedMoonInfo closestMoon;
    std::vector<GeneratedPlanetInfo> closestStarPlanets;
    std::vector<GeneratedMoonInfo> closestPlanetMoons;

};

