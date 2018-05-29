#include "stdafx.h"
#include "GalaxyContainer.h"
#include "SQLiteDatabase.h"

GalaxyContainer::GalaxyContainer()
    : bodiesNames({})
{
    onClosestStarChange = EventHandler<GeneratedStarInfo>();
    onClosestPlanetChange = EventHandler<CelestialBody>();
}


GalaxyContainer::~GalaxyContainer()
{
}

std::vector<GeneratedStarInfo>& GalaxyContainer::getAllStars()
{
    return allStars;
}

GeneratedStarInfo GalaxyContainer::getClosestStar()
{
    return closestStar;
}

CelestialBody GalaxyContainer::getClosestPlanet()
{
    return closestPlanet;
}

CelestialBody GalaxyContainer::getClosestMoon()
{
    return closestMoon;
}

CelestialBody GalaxyContainer::getClosestCelestialBody()
{
    return closestCelestialBody;
}

std::vector<CelestialBody>& GalaxyContainer::getClosestStarPlanets()
{
    return closestStarPlanets;
}

std::vector<CelestialBody>& GalaxyContainer::getClosestPlanetMoons()
{
    return closestPlanetMoons;
}

glm::dvec3 GalaxyContainer::getGravity(glm::dvec3 observerPosition, double atTime)
{
    auto flux = glm::dvec3(0.0);
    flux += closestStar.getGravity(observerPosition, atTime);
    flux += closestPlanet.getGravity(observerPosition, atTime);
    auto moons = getClosestPlanetMoons();
    for (int i = 0; i < moons.size(); i++) {
        flux += moons[i].getGravity(observerPosition, atTime);
    }
    return flux;
}

size_t GalaxyContainer::getStarsCount()
{
    return allStars.size();
}

#define asstring(a) std::to_string(a)
#define asdouble(a) std::stod(a)
#define asfloat(a) std::stof(a)
#define asint(a) std::stoi(a)
#define aslong(a) std::stol(a)
#define aslonglong(a) std::stoll(a)
void GalaxyContainer::loadFromDatabase(SQLiteDatabase * db)
{

    database = db;
    allStars = {};
    auto starsdata = database->query("SELECT * FROM stars ORDER BY id ASC");
    for (int i = 0; i < starsdata.size(); i++) {
        auto starrow = starsdata[i];
        auto star = GeneratedStarInfo();
        star.starId = aslonglong(starrow["id"]);
        star.x = aslonglong(starrow["x"]);
        star.y = aslonglong(starrow["y"]);
        star.z = aslonglong(starrow["z"]);
        star.seed = aslonglong(starrow["seed"]);
        star.radius = asdouble(starrow["radius"]);
        star.color.r = asdouble(starrow["color_r"]);
        star.color.g = asdouble(starrow["color_g"]);
        star.color.b = asdouble(starrow["color_b"]);
        star.age = asdouble(starrow["age"]);
        star.spotsIntensity = asdouble(starrow["spots_intensity"]);
        star.rotationSpeed = asdouble(starrow["rotation_speed"]);
        star.orbitPlane.x = asdouble(starrow["orbit_plane_x"]);
        star.orbitPlane.y = asdouble(starrow["orbit_plane_y"]);
        star.orbitPlane.z = asdouble(starrow["orbit_plane_z"]);
        allStars.push_back(star);
    }

    auto countbodydata = asint(database->query("SELECT count(*) FROM bodies")[0]["count(*)"]);
    countbodydata += allStars.size();

    ifstream file(Media::getPath("randomized_names.txt"));
    int i = 0;
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line) && i < countbodydata)
        {
            bodiesNames.push_back(line);
            i++;
        }
    }
}

void GalaxyContainer::update(glm::dvec3 observerPosition)
{
    if (disableBodyUpdates) return;
    updateClosestStar(observerPosition);
    if (lastStarId != closestStar.starId) {
        lastStarId = closestStar.starId;
        closestStarPlanets = loadPlanetsByStar(closestStar);
        onClosestStarChange.invoke(closestStar);
    }
    updateClosestPlanet(observerPosition);
    if (lastPlanetId != closestPlanet.bodyId) {
        lastPlanetId = closestPlanet.bodyId;
        closestPlanetMoons = loadMoonsByPlanet(closestPlanet);
        onClosestPlanetChange.invoke(closestPlanet);
    }
    updateClosestMoon(observerPosition);
    if (lastMoonId != closestMoon.bodyId) {
        lastMoonId = closestMoon.bodyId;
        onClosestMoonChange.invoke(closestMoon);
    }
    updateClosestCelestialBody(observerPosition);
}

std::string GalaxyContainer::getStarName(int id)
{
    printf("\nSize %d\n", bodiesNames.size());
    if (bodiesNames.size() < id) return "";
    return bodiesNames[id];
}

std::string GalaxyContainer::getCelestialBodyName(int id)
{
    printf("\nSize %d\n", bodiesNames.size());
    if (bodiesNames.size() < allStars.size() + id) return "";
    return bodiesNames[allStars.size() + id];
}

std::vector<CelestialBody> GalaxyContainer::loadPlanetsByStar(GeneratedStarInfo& star)
{
    auto planets = std::vector<CelestialBody>();
    auto planetsdata = database->query("SELECT * FROM bodies WHERE parentid = 0 AND starid = " + std::to_string(star.starId));
    for (int i = 0; i < planetsdata.size(); i++) {
        auto planetrow = planetsdata[i];
        auto planet = CelestialBody();
        planet.bodyId = aslong(planetrow["id"]);
        planet.host = &star;
        planet.starhost = static_cast<GeneratedStarInfo*>(&star);
        planet.radius = asdouble(planetrow["radius"]);
        planet.terrainMaxLevel = asdouble(planetrow["terrain_max"]);
        planet.fluidMaxLevel = asdouble(planetrow["fluid_max"]);
        planet.hostDistance = asdouble(planetrow["host_distance"]);
        planet.habitableChance = asdouble(planetrow["habitable_chance"]);
        planet.orbitSpeed = star.calculateOrbitVelocity(planet.hostDistance);// asdouble(planetrow["orbit_speed"]);
        planet.preferredColor.x = asfloat(planetrow["base_color_r"]);
        planet.preferredColor.y = asfloat(planetrow["base_color_g"]);
        planet.preferredColor.z = asfloat(planetrow["base_color_b"]);
        planet.atmosphereRadius = asdouble(planetrow["atmosphere_height"]);
        planet.atmosphereAbsorbStrength = asdouble(planetrow["atmosphere_absorption_strength"]);
        planet.atmosphereAbsorbColor.r = asfloat(planetrow["atmosphere_absorption_r"]);
        planet.atmosphereAbsorbColor.g = asfloat(planetrow["atmosphere_absorption_g"]);
        planet.atmosphereAbsorbColor.b = asfloat(planetrow["atmosphere_absorption_b"]);
        planet.orbitPlane.x = asfloat(planetrow["orbit_plane_x"]);
        planet.orbitPlane.y = asfloat(planetrow["orbit_plane_y"]);
        planet.orbitPlane.z = asfloat(planetrow["orbit_plane_z"]);
        planet.rotationSpeed = asdouble(planetrow["rotation_speed"]);
        planet.rotationPlane.x = asfloat(planetrow["rotation_plane_x"]);
        planet.rotationPlane.y = asfloat(planetrow["rotation_plane_y"]);
        planet.rotationPlane.z = asfloat(planetrow["rotation_plane_z"]);
        planets.push_back(planet);
    }
    return planets;
}

std::vector<CelestialBody> GalaxyContainer::loadMoonsByPlanet(CelestialBody& planet)
{
    auto moons = std::vector<CelestialBody>();
    auto moonsdata = database->query("SELECT * FROM bodies WHERE parentid = " + std::to_string(planet.bodyId));
    for (int i = 0; i < moonsdata.size(); i++) {
        auto moonrow = moonsdata[i];
        auto moon = CelestialBody();
        moon.bodyId = aslong(moonrow["id"]);
        moon.host = &planet;
        moon.starhost = static_cast<GeneratedStarInfo*>(planet.host);
        moon.radius = asdouble(moonrow["radius"]);
        moon.terrainMaxLevel = asdouble(moonrow["terrain_max"]);
        moon.fluidMaxLevel = asdouble(moonrow["fluid_max"]);
        moon.hostDistance = asdouble(moonrow["host_distance"]);
        moon.habitableChance = asdouble(moonrow["habitable_chance"]);
        moon.orbitSpeed = planet.calculateOrbitVelocity(moon.hostDistance);// asdouble(moonrow["orbit_speed"]);
        moon.preferredColor.x = asfloat(moonrow["base_color_r"]);
        moon.preferredColor.y = asfloat(moonrow["base_color_g"]);
        moon.preferredColor.z = asfloat(moonrow["base_color_b"]);
        moon.atmosphereRadius = asdouble(moonrow["atmosphere_height"]);
        moon.atmosphereAbsorbStrength = asdouble(moonrow["atmosphere_absorption_strength"]);
        moon.atmosphereAbsorbColor.r = asfloat(moonrow["atmosphere_absorption_r"]);
        moon.atmosphereAbsorbColor.g = asfloat(moonrow["atmosphere_absorption_g"]);
        moon.atmosphereAbsorbColor.b = asfloat(moonrow["atmosphere_absorption_b"]);
        moon.orbitPlane.x = asfloat(moonrow["orbit_plane_x"]);
        moon.orbitPlane.y = asfloat(moonrow["orbit_plane_y"]);
        moon.orbitPlane.z = asfloat(moonrow["orbit_plane_z"]);
        moon.rotationSpeed = asdouble(moonrow["rotation_speed"]);
        moon.rotationPlane.x = asfloat(moonrow["rotation_plane_x"]);
        moon.rotationPlane.y = asfloat(moonrow["rotation_plane_y"]);
        moon.rotationPlane.z = asfloat(moonrow["rotation_plane_z"]);
        moons.push_back(moon);
    }
    return moons;
}

void GalaxyContainer::updateClosestStar(glm::dvec3 observerPosition)
{
    GeneratedStarInfo result;
    double closestDistance = 99999999999999.0;
    for (int s = 0; s < allStars.size(); s++) {
        auto star = allStars[s];

        glm::dvec3 pos = star.getPosition(glfwGetTime());
        glm::dvec3 relpos = pos - observerPosition;

        double dst = glm::length(relpos);
        if (dst < closestDistance) {
            closestDistance = dst;
            result = star;
        }
    }
    closestStar = result;
}

void GalaxyContainer::updateClosestPlanet(glm::dvec3 observerPosition)
{
    CelestialBody result;
    double closestDistance = 99999999999999.0;
    for (int s = 0; s < closestStarPlanets.size(); s++) {
        auto planet = closestStarPlanets[s];

        glm::dvec3 pos = planet.getPosition(glfwGetTime());
        glm::dvec3 relpos = pos - observerPosition;

        double dst = glm::length(relpos);
        if (dst < closestDistance) {
            closestDistance = dst;
            result = planet;
        }
    }
    closestPlanet = result;
}

void GalaxyContainer::updateClosestMoon(glm::dvec3 observerPosition)
{
    CelestialBody result;
    double closestDistance = 99999999999999.0;
    for (int s = 0; s < closestPlanetMoons.size(); s++) {
        auto moon = closestPlanetMoons[s];

        glm::dvec3 pos = moon.getPosition(glfwGetTime());
        glm::dvec3 relpos = pos - observerPosition;

        double dst = glm::length(relpos);
        if (dst < closestDistance) {
            closestDistance = dst;
            result = moon;
        }
    }
    closestMoon = result;
}

void GalaxyContainer::updateClosestCelestialBody(glm::dvec3 observerPosition)
{
    CelestialBody result;
    double closestDistance = 99999999999999.0;
    for (int s = 0; s < closestStarPlanets.size(); s++) {
        auto planet = closestStarPlanets[s];

        glm::dvec3 pos = planet.getPosition(glfwGetTime());
        glm::dvec3 relpos = pos - observerPosition;

        double dst = glm::length(relpos);
        if (dst < closestDistance) {
            closestDistance = dst;
            result = planet;
        }
    }
    for (int s = 0; s < closestPlanetMoons.size(); s++) {
        auto moon = closestPlanetMoons[s];

        glm::dvec3 pos = moon.getPosition(glfwGetTime());
        glm::dvec3 relpos = pos - observerPosition;

        double dst = glm::length(relpos);
        if (dst < closestDistance) {
            closestDistance = dst;
            result = moon;
        }
    }
    closestCelestialBody = result;
}

AbsCelestialObject * GalaxyContainer::getByPath(int starId, int planetId, int moonId)
{
    auto star = getAllStars()[starId - 1];
    if (planetId <= 0) return &star;
    GeneratedStarInfo* starinfo = new GeneratedStarInfo();
    *starinfo = star;
    auto celestialTarget = loadPlanetsByStar(star)[planetId - 1];
    auto planet = new CelestialBody();
    *planet = celestialTarget;
    planet->host = starinfo;
    if (moonId <= 0) return planet;
    auto moonCelestialTarget = loadMoonsByPlanet(*planet)[moonId - 1];
    auto moon = new CelestialBody();
    *moon = moonCelestialTarget;
    moon->host = planet;
    return moon;
}

void GalaxyContainer::setBodyUpdates(bool enable)
{
    disableBodyUpdates = enable;
}