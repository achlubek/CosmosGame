#include "stdafx.h"
#include "GalaxyGenerator.h"

GalaxyGenerator::GalaxyGenerator()
{
    eng = std::mt19937_64(rd());
}


GalaxyGenerator::~GalaxyGenerator()
{
}

uint64_t GalaxyGenerator::rand_uint64(uint64_t min, uint64_t max) {

    std::uniform_int_distribution<uint64_t> distr = std::uniform_int_distribution<uint64_t>(min, max);
    return distr(eng);
}

int64_t GalaxyGenerator::rand_int64(int64_t min, int64_t max) {

    std::uniform_int_distribution<int64_t> distr = std::uniform_int_distribution<int64_t>(min, max);
    return distr(eng);
}
 

double GalaxyGenerator::rand_normalized_double() {
    std::uniform_int_distribution<uint64_t> distr = std::uniform_int_distribution<uint64_t>();
    return ((double)distr(eng)) / ((double)UINT64_MAX);
}
glm::dvec2 rotate(glm::dvec2 v, double a) {
    double s = sin(a);
    double c = cos(a);
    glm::dmat2 m = glm::dmat2(c, -s, s, c);
    return m * v;
}
double hashx(double n) {
    return glm::fract(sin(n)*758.5453);
}
GeneratedStarSystemInfo* GalaxyGenerator::generateStar(int64_t galaxyradius, int64_t centerThickness, double centerGravity, uint64_t seed)
{ 
    eng.seed(seed);
    eng.seed(rand_int64(0, UINT64_MAX));
    auto system = new GeneratedStarSystemInfo();
    Star* star = new Star();
    star->seed = seed;

#define drand2rn() (rand_normalized_double() * 2.0 - 1.0)
    float x = rand_normalized_double();
    float y = rand_normalized_double();
    float w = rand_normalized_double();

    glm::dvec2 c = glm::dvec2(x, y);
    c = c * 2.0 - 1.0;
    c *= w * w;
    c *= glm::dvec2(0.5, 1.0);
    c = rotate(c, w * 13.2340);

    double dst = glm::length(c);

    double thickness = (1.0 / (dst * 2.0 + 1.0));

    star->x = static_cast<int64_t>(static_cast<double>(galaxyradius) * c.x);
    star->z = static_cast<int64_t>(static_cast<double>(galaxyradius) * c.y);
    star->y = static_cast<int64_t>((centerThickness) * (thickness * (drand2rn() * drand2rn() * drand2rn())));
    star->radius = static_cast<double>(rand_uint64(19570, 169570));
    glm::dvec3 red = glm::dvec3(1.0, 0.0, 0.0);
    glm::dvec3 yellow = glm::dvec3(1.0, 1.0, 0.9);
    glm::dvec3 blue = glm::dvec3(0.0, 0.0, 1.0);
    double randcolor = rand_normalized_double();
    glm::dvec3 starcolor = glm::mix(glm::mix(red, yellow, glm::min(1.0, randcolor * 2.0)), blue, glm::max(0.0, (randcolor - 0.5) * 2.0));
    star->color = starcolor;
    star->age = rand_normalized_double();
    star->spotsIntensity = rand_normalized_double();
    star->rotationSpeed = rand_normalized_double();
    star->orbitPlane = glm::normalize(glm::vec3(rand_normalized_double(), rand_normalized_double(), rand_normalized_double()) * 2.0f - 1.0f);
    star->planetsCount = rand_uint64(4, 10);
    system->star = star;
    system->bodies = {};

    for (int i = 0; i < star->planetsCount; i++) {
        CelestialBody* planet = new CelestialBody();
        planet->bodyId = i;
        // planet->host = star;
        double orbitDisturb = rand_normalized_double() * 0.25;
        planet->hostDistance = (glm::pow(1.6 + orbitDisturb, i+1) * (5791000.0 / (1.6 + orbitDisturb))) * 0.1;
        
        
        // arra[i];
       // tmpHostDistance += 4160000.0 + rand_normalized_double() * rand_normalized_double() * 158440000.0;
        // stardisthelper += rand_uint64(4000000, 162600000);

        uint64_t habitableStart = 1000000;// 1082000;
        uint64_t habitableEnd = 2580000;// 3279000;
        int moonsCount = 0;
        if (planet->hostDistance < habitableStart) {
            // Rocky and small ONLY
            planet->radius = static_cast<double>(rand_uint64(1140, 3440)); // ranges from mercury to roughly 2x mercury
            moonsCount = static_cast<int>(rand_uint64(1, 2));
            planet->atmosphereRadius = 0.0;
            planet->atmosphereAbsorbStrength = 0.0;
            planet->atmosphereAbsorbColor = glm::vec3(0.0);
            planet->terrainMaxLevel = rand_normalized_double() * planet->radius * 0.006;
            planet->fluidMaxLevel = 0.0;
            planet->habitableChance = 0.0;
            planet->preferredColor = glm::vec3(rand_normalized_double(), rand_normalized_double(), rand_normalized_double());
        }
        else if (planet->hostDistance >= habitableStart && planet->hostDistance <= habitableEnd) {
            // earth like or venus/mars like
            planet->radius = static_cast<double>(rand_uint64(3371.0, 8371.0)); // ranges from mars to 1,5x earth
            moonsCount = rand_uint64(1, 4);
            planet->atmosphereRadius = (planet->radius * 0.015) * (0.5 + rand_normalized_double());
            planet->terrainMaxLevel = planet->radius * 0.012 * (0.3 + rand_normalized_double());
            float rand1 = rand_normalized_double();
            
            if (rand1 < 0.5) {
                // oxygen etc, blue marbles
                planet->atmosphereAbsorbStrength = 0.22 * rand_normalized_double();
                planet->atmosphereAbsorbColor = glm::vec3(rand_normalized_double() * rand_normalized_double() * rand_normalized_double(), sqrt(rand_normalized_double()), rand_normalized_double());
                planet->fluidMaxLevel = planet->terrainMaxLevel - sqrt(rand_normalized_double()) * planet->terrainMaxLevel;
                planet->habitableChance = rand_normalized_double();
                planet->preferredColor = planet->atmosphereAbsorbColor * 0.5f + 0.5f * glm::vec3(rand_normalized_double(), rand_normalized_double(), rand_normalized_double());
            }
            else {
                // some mars like
                planet->atmosphereAbsorbStrength = 0.1 * rand_normalized_double();
                planet->atmosphereAbsorbColor = glm::vec3(0.2 + rand_normalized_double()* 0.8, 0.1 + rand_normalized_double()* 0.9, 0.7 + rand_normalized_double()* 0.3);
                planet->fluidMaxLevel = 0.0;
                planet->habitableChance = rand_normalized_double() * rand_normalized_double() * rand_normalized_double();
                planet->preferredColor = glm::vec3(0.1 + rand_normalized_double() * 0.9, 0.1 + rand_normalized_double() * 0.9, 0.1 + rand_normalized_double() * 0.9);
            }
        }
        else {
            // gaseous giants and small rocky shits
            float rand1 = rand_normalized_double();
            if (rand1 < 0.8) {
                // gaseous giant
                planet->radius = static_cast<double>(rand_uint64(25360.2, 69910.1)); // ranges from uranus to jupiter
                moonsCount = rand_uint64(5, 16);
                planet->atmosphereRadius = (rand_normalized_double() + 2.0) * (planet->radius * 0.1);
                planet->atmosphereAbsorbStrength = 0.7 + 0.3 * rand_normalized_double();
                planet->atmosphereAbsorbColor = glm::vec3(0.1 + rand_normalized_double() * 0.9, 0.1 + rand_normalized_double() * 0.9, 0.1 + rand_normalized_double() * 0.9);
                planet->terrainMaxLevel = 0.0;
                planet->fluidMaxLevel = 0.0;
                planet->habitableChance = 0.0;
                planet->preferredColor = glm::vec3(0.1 + rand_normalized_double() * 0.9, 0.1 + rand_normalized_double() * 0.9, 0.1 + rand_normalized_double() * 0.9);
            }
            else {
                // rocky
                planet->radius = static_cast<double>(rand_uint64(2440.0, 5440.0)); // ranges from mercury to roughly 2x mercury
                moonsCount = rand_uint64(1, 2);
                planet->atmosphereRadius = 0.0;
                planet->atmosphereAbsorbStrength = 0.0;
                planet->atmosphereAbsorbColor = glm::vec3(0.0);
                planet->terrainMaxLevel = rand_normalized_double() * planet->radius * 0.0064;
                planet->fluidMaxLevel = 0.0;
                planet->habitableChance = 0.0;
                planet->preferredColor = glm::vec3(rand_normalized_double(), rand_normalized_double(), rand_normalized_double());
            }
        }

        planet->orbitSpeed = 0;// 1000000.0 * rand_normalized_double() / planet->hostDistance;
        planet->orbitPlane = glm::normalize(glm::vec3(rand_normalized_double(), rand_normalized_double(), rand_normalized_double()) * 2.0f - 1.0f);
        planet->rotationSpeed = rand_normalized_double();
        planet->rotationPlane = glm::normalize(glm::dvec3((rand_normalized_double() * 0.3f) * 2.0f - 1.0f, 1.0f, (rand_normalized_double() * 0.3f) * 2.0f - 1.0f));
        system->bodies.push_back(planet);
        for (int g = 0; g < moonsCount; g++) {
            CelestialBody* moon = new CelestialBody();

            bool isAtmospheric = rand_normalized_double() > 0.8;
            moon->host = planet;
            moon->radius = static_cast<double>(rand_uint64(440.0, 5240.0)); // something small to ganymede diameter, might be overkill
            moon->terrainMaxLevel = moon->radius * 0.01;
            if (isAtmospheric) {
                moon->habitableChance = rand_normalized_double();
                moon->fluidMaxLevel = moon->terrainMaxLevel - moon->terrainMaxLevel * sqrt(rand_normalized_double());
                moon->atmosphereRadius = (moon->terrainMaxLevel * 2.0);
                moon->atmosphereAbsorbStrength = 0.7 + 0.3 * rand_normalized_double();
                moon->atmosphereAbsorbColor = glm::vec3(0.1 + rand_normalized_double() * 0.9, 0.1 + rand_normalized_double() * 0.9, 0.1 + rand_normalized_double() * 0.9);
                moon->preferredColor = glm::vec3(rand_normalized_double(), rand_normalized_double(), rand_normalized_double());
            }
            else {
                moon->fluidMaxLevel = 0.0;
                moon->habitableChance = 0.0;
                moon->atmosphereRadius = 0.0;
                moon->atmosphereAbsorbStrength = 0.0;
                moon->atmosphereAbsorbColor = glm::vec3(0.0);
                moon->preferredColor = glm::vec3(rand_normalized_double(), rand_normalized_double(), rand_normalized_double());
            }
            moon->orbitPlane = glm::normalize(glm::vec3(rand_normalized_double(), rand_normalized_double(), rand_normalized_double()) * 2.0f - 1.0f);
            moon->hostDistance = planet->radius * 40.0 * (((float)g) * 0.2 + 1.0);
            moon->orbitSpeed = 0;// 1000000.0 * rand_normalized_double() / moon->hostDistance;
            moon->rotationSpeed = rand_normalized_double();
            moon->rotationPlane = glm::normalize(glm::dvec3((rand_normalized_double() * 0.3f) * 2.0f - 1.0f, 1.0f, (rand_normalized_double() * 0.3f) * 2.0f - 1.0f));
            system->bodies.push_back(moon);
        }
    }
    return system;
}
  