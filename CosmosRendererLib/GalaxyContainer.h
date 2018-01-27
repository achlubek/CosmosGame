#pragma once
#include "GeneratedStarInfo.h"
#include "GeneratedPlanetInfo.h"
#include "GeneratedMoonInfo.h"
class SQLiteDatabase;
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
    SQLiteDatabase* database;
    std::vector<GeneratedStarInfo> allStars;
    GeneratedStarInfo closestStar;
    GeneratedPlanetInfo closestPlanet;
    std::vector<GeneratedPlanetInfo> closestStarPlanets;
    std::vector<GeneratedMoonInfo> closestPlanetMoons;

    std::vector<GeneratedPlanetInfo> loadPlanetsByStar(GeneratedStarInfo star);
    std::vector<GeneratedMoonInfo> loadMoonsByPlanet(GeneratedPlanetInfo planet);
    void updateClosestStar(glm::dvec3 observerPosition);
    void updateClosestPlanet(glm::dvec3 observerPosition);
    uint64_t lastStarId = 0;
    uint64_t lastPlanetId = 0;
};

