#include "stdafx.h"
#include "GalaxyContainer.h"
#include "SQLiteDatabase.h"

GalaxyContainer::GalaxyContainer()
{
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

GeneratedPlanetInfo GalaxyContainer::getClosestPlanet()
{
    return closestPlanet;
}

std::vector<GeneratedPlanetInfo>& GalaxyContainer::getClosestStarPlanets()
{
    return closestStarPlanets;
}

std::vector<GeneratedMoonInfo>& GalaxyContainer::getClosestPlanetMoons()
{
    return closestPlanetMoons;
}

glm::dvec3 GalaxyContainer::getGravity(glm::dvec3 observerPosition)
{
    return glm::dvec3();
}

long GalaxyContainer::getStarsCount()
{
    return allStars.size();
}

#define asstring(a) std::to_string(a)
#define asdouble(a) std::stod(a)
#define asint(a) std::stoi(a)
#define aslong(a) std::stol(a)
void GalaxyContainer::loadFromDatabase(SQLiteDatabase * db)
{
    database = db;
    allStars = {};
    auto starsdata = database->query("SELECT * FROM stars ORDER BY id ASC");
    for (int i = 0; i < starsdata.size(); i++) {
        auto starrow = starsdata[i];
        auto star = GeneratedStarInfo();
        star.starId = aslong(starrow["id"]);
        star.x = asdouble(starrow["x"]);
        star.y = asdouble(starrow["y"]);
        star.z = asdouble(starrow["z"]);
        star.seed = asdouble(starrow["seed"]);
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
}

void GalaxyContainer::update(glm::dvec3 observerPosition)
{
    updateClosestStar(observerPosition);
    if (lastStarId != closestStar.starId) {
        lastStarId = closestStar.starId;
        closestStarPlanets = loadPlanetsByStar(closestStar);
    }
    updateClosestPlanet(observerPosition);
    if (lastPlanetId != closestPlanet.planetId) {
        lastPlanetId = closestPlanet.planetId;
        closestPlanetMoons = loadMoonsByPlanet(closestPlanet);
    }
}

std::vector<GeneratedPlanetInfo> GalaxyContainer::loadPlanetsByStar(GeneratedStarInfo star)
{
    auto planets = std::vector<GeneratedPlanetInfo>();
    auto planetsdata = database->query("SELECT * FROM planets WHERE starid = " + std::to_string(star.starId));
    for (int i = 0; i < planetsdata.size(); i++) {
        auto planetrow = planetsdata[i];
        auto planet = GeneratedPlanetInfo();
        planet.planetId = aslong(planetrow["id"]);
        planet.host = star;
        planet.radius = asdouble(planetrow["radius"]);
        planet.terrainMaxLevel = asdouble(planetrow["terrain_max"]);
        planet.fluidMaxLevel = asdouble(planetrow["fluid_max"]);
        planet.starDistance = asdouble(planetrow["star_distance"]);
        planet.habitableChance = asdouble(planetrow["habitable_chance"]);
        planet.orbitSpeed = asdouble(planetrow["orbit_speed"]);
        planet.preferredColor.x = asdouble(planetrow["base_color_r"]);
        planet.preferredColor.y = asdouble(planetrow["base_color_g"]);
        planet.preferredColor.z = asdouble(planetrow["base_color_b"]);
        planet.atmosphereRadius = asdouble(planetrow["atmosphere_height"]);
        planet.atmosphereAbsorbStrength = asdouble(planetrow["atmosphere_absorption_strength"]);
        planet.atmosphereAbsorbColor.r = asdouble(planetrow["atmosphere_absorption_r"]);
        planet.atmosphereAbsorbColor.g = asdouble(planetrow["atmosphere_absorption_g"]);
        planet.atmosphereAbsorbColor.b = asdouble(planetrow["atmosphere_absorption_b"]);
        planets.push_back(planet);
    }
    printf("loaded %d planets\n", planets.size());
    return planets;
}

std::vector<GeneratedMoonInfo> GalaxyContainer::loadMoonsByPlanet(GeneratedPlanetInfo planet)
{
    auto moons = std::vector<GeneratedMoonInfo>();
    auto moonsdata = database->query("SELECT * FROM moons WHERE planetid = " + std::to_string(planet.planetId));
    for (int i = 0; i < moonsdata.size(); i++) {
        auto moonrow = moonsdata[i];
        auto moon = GeneratedMoonInfo();
        moon.moonId = aslong(moonrow["id"]);
        moon.host = planet;
        moon.radius = asdouble(moonrow["radius"]);
        moon.terrainMaxLevel = asdouble(moonrow["terrain_max"]);
        moon.fluidMaxLevel = asdouble(moonrow["fluid_max"]);
        moon.planetDistance = asdouble(moonrow["planet_distance"]);
        moon.habitableChance = asdouble(moonrow["habitable_chance"]);
        moon.orbitSpeed = asdouble(moonrow["orbit_speed"]);
        moon.preferredColor.x = asdouble(moonrow["base_color_r"]);
        moon.preferredColor.y = asdouble(moonrow["base_color_g"]);
        moon.preferredColor.z = asdouble(moonrow["base_color_b"]);
        moon.atmosphereRadius = asdouble(moonrow["atmosphere_height"]);
        moon.atmosphereAbsorbStrength = asdouble(moonrow["atmosphere_absorption_strength"]);
        moon.atmosphereAbsorbColor.r = asdouble(moonrow["atmosphere_absorption_r"]);
        moon.atmosphereAbsorbColor.g = asdouble(moonrow["atmosphere_absorption_g"]);
        moon.atmosphereAbsorbColor.b = asdouble(moonrow["atmosphere_absorption_b"]);
        moon.orbitPlane.x = asdouble(moonrow["orbit_plane_x"]);
        moon.orbitPlane.y = asdouble(moonrow["orbit_plane_y"]);
        moon.orbitPlane.z = asdouble(moonrow["orbit_plane_z"]);
        moons.push_back(moon);
    }
    printf("loaded %d moons\n", moons.size());
    return moons;
}

void GalaxyContainer::updateClosestStar(glm::dvec3 observerPosition)
{
    GeneratedStarInfo result;
    double closestDistance = 9999999999.0;
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
    GeneratedPlanetInfo result;
    double closestDistance = 9999999999.0;
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
