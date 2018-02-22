#include "stdafx.h"
#include "GalaxyGenerator.h"

#include <random>
#include <limits>

GalaxyGenerator::GalaxyGenerator()
{
    eng = std::mt19937_64(rd());
}


GalaxyGenerator::~GalaxyGenerator()
{
}

uint64_t GalaxyGenerator::randu64(uint64_t min, uint64_t max) {

    std::uniform_int_distribution<uint64_t> distr = std::uniform_int_distribution<uint64_t>(min, max);
    return distr(eng);
}

int64_t GalaxyGenerator::randi64(int64_t min, int64_t max) {

    std::uniform_int_distribution<int64_t> distr = std::uniform_int_distribution<int64_t>(min, max);
    return distr(eng);
}
 

double GalaxyGenerator::drandnorm() {
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
    eng.seed(randi64(0, UINT64_MAX));
    auto system = new GeneratedStarSystemInfo();
    GeneratedStarInfo* star = new GeneratedStarInfo();
    star->seed = seed;
    double ss = drandnorm() * 100.0;
#define rnd() hashx(ss);ss+=100.0;

    double lu = drandnorm();
    double x = drandnorm();
    double y = drandnorm();
    double rx = drandnorm() * 2.0 - 1.0;
    double ry = drandnorm() * 2.0 - 1.0;
    double w = drandnorm();
#define drand2rn() (drandnorm() * 2.0 - 1.0)
    double a = 1.7;
    double coef1 = 1.0 / (sqrt(2.0 * 3.1415 * a * a));
    double coef2 = -1.0 / (2.0 * a * a);

    double gauss0 = coef1 * pow(2.7182818, pow(w * 1.0, 2.0) * coef2);
    glm::dvec2 c = glm::dvec2(x, y);
    c = c * 2.0 - 1.0;
    c = glm::normalize(c) *glm::dvec2(0.92, 1.0);
    c *= w * w; // if > 0.1 then * 
    c = rotate(c, w * 13.2340);

    double dst = glm::length(c);

    c += glm::dvec2(rx, ry) * (0.01 / (0.01 + dst * 1000.0));

    double gauss1 = cos(3.1415 * 0.5 * (abs(dst)));// coef1 * pow(2.7182818, pow(dst * 0.01, 2.0) * coef2);
    double gauss2 = coef1 * pow(2.7182818, pow(drandnorm() * 2.0, 2.0) * coef2);
    // point c is in -1 -> 1
    w = 1.0 - w;
    lu = w*w*(3.0 - 2.0 * w);
    star->x = static_cast<double>(galaxyradius) * c.x;
    star->z = static_cast<double>(galaxyradius) * c.y;
    star->y = static_cast<double>(centerThickness) * (gauss1 * (drand2rn() * drand2rn() * drand2rn()));
    star->radius = randu64(39100, 139100);
    star->color = glm::vec3(0.5 + drandnorm() * drandnorm(), 0.5 + drandnorm() * drandnorm(), 0.5 + drandnorm() * drandnorm());
    star->age = drandnorm();
    star->spotsIntensity = drandnorm();
    star->rotationSpeed = drandnorm();
    star->orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
    star->planetsCount = randu64(8, 8); 
    system->star = star;
    system->bodies = {};
    //double stardisthelper = 5800000;
    double arra[8] = { 580000,
        1080000,
        1490000,
        2280000,
        7780000,
        14270000,
        28710000,
        44970000 };

    for (int i = 0; i < star->planetsCount; i++) {
        CelestialBody* planet = new CelestialBody();
        planet->bodyId = i;
        // planet->host = star;
        planet->hostDistance = arra[i];
        // stardisthelper += randu64(4000000, 162600000);

        uint64_t habitableStart = 1080000;// 1082000;
        uint64_t habitableEnd = 2280000;// 3279000;
        int moonsCount = 0;
        if (planet->hostDistance < habitableStart) {
            // Rocky and small ONLY
            planet->radius = randu64(244, 544); // ranges from mercury to roughly 2x mercury
            moonsCount = randu64(1, 2);
            planet->atmosphereRadius = 0.0;
            planet->atmosphereAbsorbStrength = 0.0;
            planet->atmosphereAbsorbColor = glm::vec3(0.0);
            planet->terrainMaxLevel = drandnorm() * planet->radius * 0.0138;
            planet->fluidMaxLevel = 0.0;
            planet->habitableChance = 0.0;
            planet->preferredColor = glm::vec3(0.8 + drandnorm() * 0.2, 0.4 + drandnorm() * 0.3, 0.4 + drandnorm() * 0.2);

        }
        else if (planet->hostDistance >= habitableStart && planet->hostDistance <= habitableEnd) {
            // earth like or venus/mars like
            planet->radius = randu64(339, 937.1); // ranges from mars to 1,5x earth
            moonsCount = randu64(1, 4);
            planet->atmosphereRadius = (planet->radius * 0.004709);
            planet->terrainMaxLevel =  planet->radius * 0.04709;
            float rand1 = drandnorm();
            planet->atmosphereAbsorbStrength = 0.12;
            planet->atmosphereAbsorbColor = glm::vec3(drandnorm(), drandnorm(), drandnorm());
            planet->fluidMaxLevel = drandnorm() * planet->terrainMaxLevel;
            planet->habitableChance = drandnorm();
            planet->preferredColor = glm::vec3(drandnorm(), drandnorm(), drandnorm());
            /*
            if (rand1 < 0.5) {
                // oxygen etc, blue marbles
                planet->atmosphereAbsorbStrength = 0.12;
                planet->atmosphereAbsorbColor = glm::vec3(0.24, 0.58, 1.0);
                planet->fluidMaxLevel = drandnorm() * planet->terrainMaxLevel;
                planet->habitableChance = 1.0;
                planet->preferredColor = glm::vec3(0.2 + drandnorm() * 0.2, 0.7 + drandnorm() * 0.3, 0.2 + drandnorm() * 0.2);
            }
            else {
                // some mars like
                planet->atmosphereAbsorbStrength = 0.12;
                planet->atmosphereAbsorbColor = glm::vec3(0.8 + drandnorm()* 0.2, 0.3 + drandnorm()* 0.2, 0.1 + drandnorm()* 0.1);
                planet->fluidMaxLevel = drandnorm() * planet->terrainMaxLevel;
                planet->habitableChance = 0.5;
                planet->preferredColor = glm::vec3(0.8 + drandnorm() * 0.2, 0.4 + drandnorm() * 0.3, 0.1 + drandnorm() * 0.2);
            }*/
        }
        else {
            // gaseous giants and small rocky shits
            float rand1 = drandnorm();
            if (rand1 < 0.8) {
                // gaseous giant
                planet->radius = randu64(2536.2, 6991.1); // ranges from uranus to jupiter
                moonsCount = randu64(5, 16);
                planet->atmosphereRadius = (drandnorm() + 2.0) * (planet->radius * 0.1);
                planet->atmosphereAbsorbStrength = 0.7 + 0.3 * drandnorm();
                planet->atmosphereAbsorbColor = glm::vec3(0.1 + drandnorm() * 0.8, 0.1 + drandnorm() * 0.8, 0.1 + drandnorm() * 0.8);
                planet->terrainMaxLevel = 0.0;
                planet->fluidMaxLevel = 0.0;
                planet->habitableChance = 0.0;
                planet->preferredColor = glm::vec3(0.1 + drandnorm() * 0.8, 0.1 + drandnorm() * 0.8, 0.1 + drandnorm() * 0.8);
            }
            else {
                // rocky
                planet->radius = randu64(244.0, 544.0); // ranges from mercury to roughly 2x mercury
                moonsCount = randu64(1, 2);
                planet->atmosphereRadius = 0.0;
                planet->atmosphereAbsorbStrength = 0.0;
                planet->atmosphereAbsorbColor = glm::vec3(0.0);
                planet->terrainMaxLevel = drandnorm() * planet->radius * 0.00138;
                planet->fluidMaxLevel = 0.0;
                planet->habitableChance = 0.0;
                planet->preferredColor = glm::vec3(0.8 + drandnorm() * 0.2, 0.7 + drandnorm() * 0.3, 0.7 + drandnorm() * 0.3);
            }
        }

        planet->orbitSpeed = 1000000.0 * drandnorm() / planet->hostDistance;
        planet->orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
        system->bodies.push_back(planet);
        for (int g = 0; g < moonsCount; g++) {
            CelestialBody* moon = new CelestialBody();
            moon->host = planet;
            moon->radius = randu64(planet->radius / 30, planet->radius / 15);
            moon->terrainMaxLevel =  moon->radius * 0.08138;
            moon->fluidMaxLevel = moon->terrainMaxLevel * drandnorm();
            moon->habitableChance = drandnorm();
            moon->atmosphereRadius = (moon->radius * 0.009709 * drandnorm());
            moon->atmosphereAbsorbStrength = 0.7 + 0.3 * drandnorm();
            moon->atmosphereAbsorbColor = glm::vec3(0.1 + drandnorm() * 0.8, 0.1 + drandnorm() * 0.8, 0.1 + drandnorm() * 0.8);

            moon->orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
            moon->hostDistance = planet->radius * 3.0 + planet->radius * drandnorm() * 3.0;
            moon->orbitSpeed = 1000000.0 * drandnorm() / moon->hostDistance;
            moon->preferredColor = glm::vec3(0.6 + 0.4 * drandnorm(), 0.6 + 0.4 * drandnorm(), 0.6 + 0.4 * drandnorm());
            system->bodies.push_back(moon);
        }
    }
    return system;
}
  