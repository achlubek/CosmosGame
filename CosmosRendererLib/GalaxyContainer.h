#pragma once
#include "GeneratedStarInfo.h"
#include "CelestialBody.h"
#include "RenderedCelestialBody.h"
#include "EventHandler.h"
class SQLiteDatabase;
class GalaxyContainer
{
public:
    GalaxyContainer();
    ~GalaxyContainer();
    std::vector<GeneratedStarInfo>& getAllStars();
    GeneratedStarInfo getClosestStar();
    CelestialBody getClosestPlanet();
    CelestialBody getClosestMoon();
    CelestialBody getClosestCelestialBody();
    std::vector<CelestialBody>& getClosestStarPlanets();
    std::vector<CelestialBody>& getClosestPlanetMoons();
    glm::dvec3 getGravity(glm::dvec3 observerPosition, double atTime);
    size_t getStarsCount();
    void loadFromDatabase(SQLiteDatabase* db);
    void update(glm::dvec3 observerPosition);

    AbsCelestialObject* getByPath(int starId, int planetId = 0, int moonId = 0);

    EventHandler<GeneratedStarInfo> onClosestStarChange;
    EventHandler<CelestialBody> onClosestPlanetChange;
    EventHandler<CelestialBody> onClosestMoonChange;
    void setBodyUpdates(bool enable);
private:
    SQLiteDatabase* database;
    std::vector<GeneratedStarInfo> allStars;
    GeneratedStarInfo closestStar;
    CelestialBody closestPlanet;
    CelestialBody closestMoon;
    CelestialBody closestCelestialBody;
    std::vector<CelestialBody> closestStarPlanets;
    std::vector<CelestialBody> closestPlanetMoons;

    std::vector<CelestialBody> loadPlanetsByStar(GeneratedStarInfo& star);
    std::vector<CelestialBody> loadMoonsByPlanet(CelestialBody& planet);
    void updateClosestStar(glm::dvec3 observerPosition);
    void updateClosestPlanet(glm::dvec3 observerPosition);
    void updateClosestMoon(glm::dvec3 observerPosition);
    void updateClosestCelestialBody(glm::dvec3 observerPosition);
    uint64_t lastStarId = 0;
    uint64_t lastPlanetId = 0;
    uint64_t lastMoonId = 0;
    bool disableBodyUpdates = false;
};

