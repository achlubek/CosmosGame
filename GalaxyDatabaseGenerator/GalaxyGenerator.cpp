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
    star->x = static_cast<int64_t>(static_cast<double>(galaxyradius) * c.x);
    star->z = static_cast<int64_t>(static_cast<double>(galaxyradius) * c.y);
    star->y = static_cast<int64_t>((centerThickness) * (gauss1 * (drand2rn() * drand2rn() * drand2rn())));
    star->radius = static_cast<double>(randu64(69570, 69570));
    star->color = glm::vec3(0.5 + drandnorm() * drandnorm(), 0.5 + drandnorm() * drandnorm(), 0.5 + drandnorm() * drandnorm());
    star->age = drandnorm();
    star->spotsIntensity = drandnorm();
    star->rotationSpeed = drandnorm();
    star->orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
    star->planetsCount = randu64(4, 10);
    system->star = star;
    system->bodies = {};
    //double stardisthelper = 5800000;
    /*
    double arra[9] = { 
        5791000,
        10800000, // dx 5009000
        14960000, // dx 4160000
        22800000, // dx 7840000
        77800000, // dx 55000000
        142700000, // dx 64900000
        287100000, //dx 144400000
        449700000 , //dx 162600000
        649700000 };*/
    // planet distances delta varies from 4160000 to insanes like 162600000

   // double tmpHostDistance = 2791000.0 + 3791000.0 * drandnorm();

    for (int i = 0; i < star->planetsCount; i++) {
        CelestialBody* planet = new CelestialBody();
        planet->bodyId = i;
        // planet->host = star;
        double orbitDisturb = drandnorm() * 0.25;
        planet->hostDistance = (glm::pow(1.6 + orbitDisturb, i+1) * (5791000.0 / (1.6 + orbitDisturb))) * 0.01;// arra[i];
       // tmpHostDistance += 4160000.0 + drandnorm() * drandnorm() * 158440000.0;
        // stardisthelper += randu64(4000000, 162600000);

        uint64_t habitableStart = 100000;// 1082000;
        uint64_t habitableEnd = 258000;// 3279000;
        int moonsCount = 0;
        if (planet->hostDistance < habitableStart) {
            // Rocky and small ONLY
            planet->radius = static_cast<double>(randu64(1140, 3440)); // ranges from mercury to roughly 2x mercury
            moonsCount = static_cast<int>(randu64(1, 2));
            planet->atmosphereRadius = 0.0;
            planet->atmosphereAbsorbStrength = 0.0;
            planet->atmosphereAbsorbColor = glm::vec3(0.0);
            planet->terrainMaxLevel = drandnorm() * planet->radius * 0.02;
            planet->fluidMaxLevel = 0.0;
            planet->habitableChance = 0.0;
            planet->preferredColor = glm::vec3(drandnorm(), drandnorm(), drandnorm());
        }
        else if (planet->hostDistance >= habitableStart && planet->hostDistance <= habitableEnd) {
            // earth like or venus/mars like
            planet->radius = static_cast<double>(randu64(3371.0, 8371.0)); // ranges from mars to 1,5x earth
            moonsCount = randu64(1, 4);
            planet->atmosphereRadius = (planet->radius * 0.02);
            planet->terrainMaxLevel =  planet->radius * 0.02;
            float rand1 = drandnorm();
            planet->atmosphereAbsorbStrength = 0.12;
            planet->atmosphereAbsorbColor = glm::vec3(drandnorm() * drandnorm() * drandnorm(), sqrt(drandnorm()), drandnorm());
            planet->fluidMaxLevel = planet->terrainMaxLevel - sqrt(drandnorm()) * planet->terrainMaxLevel;
            planet->habitableChance = drandnorm();
            planet->preferredColor = planet->atmosphereAbsorbColor * 0.5f + 0.5f * glm::vec3(drandnorm(), drandnorm(), drandnorm());
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
                planet->radius = randu64(25360.2, 69910.1); // ranges from uranus to jupiter
                moonsCount = randu64(5, 16);
                planet->atmosphereRadius = (drandnorm() + 2.0) * (planet->radius * 0.1);
                planet->atmosphereAbsorbStrength = 0.7 + 0.3 * drandnorm();
                planet->atmosphereAbsorbColor = glm::vec3(0.1 + drandnorm() * 0.9, 0.1 + drandnorm() * 0.9, 0.1 + drandnorm() * 0.9);
                planet->terrainMaxLevel = 0.0;
                planet->fluidMaxLevel = 0.0;
                planet->habitableChance = 0.0;
                planet->preferredColor = glm::vec3(0.1 + drandnorm() * 0.9, 0.1 + drandnorm() * 0.9, 0.1 + drandnorm() * 0.9);
            }
            else {
                // rocky
                planet->radius = randu64(2440.0, 5440.0); // ranges from mercury to roughly 2x mercury
                moonsCount = randu64(1, 2);
                planet->atmosphereRadius = 0.0;
                planet->atmosphereAbsorbStrength = 0.0;
                planet->atmosphereAbsorbColor = glm::vec3(0.0);
                planet->terrainMaxLevel = drandnorm() * planet->radius * 0.0064;
                planet->fluidMaxLevel = 0.0;
                planet->habitableChance = 0.0;
                planet->preferredColor = glm::vec3(drandnorm(), drandnorm(), drandnorm());
            }
        }

        planet->orbitSpeed = 1000000.0 * drandnorm() / planet->hostDistance;
        planet->orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
        planet->rotationSpeed = drandnorm();
        planet->rotationPlane = glm::normalize(glm::dvec3((drandnorm() * 0.3f) * 2.0f - 1.0f, 1.0f, (drandnorm() * 0.3f) * 2.0f - 1.0f));
        system->bodies.push_back(planet);
        for (int g = 0; g < moonsCount; g++) {
            CelestialBody* moon = new CelestialBody();

            bool isAtmospheric = drandnorm() > 0.8;
            moon->host = planet;
            moon->radius = randu64(planet->radius / 30, planet->radius / 15);
            moon->terrainMaxLevel = moon->radius * 0.02;
            if (isAtmospheric) {
                moon->habitableChance = drandnorm();
                moon->fluidMaxLevel = moon->terrainMaxLevel - moon->terrainMaxLevel * sqrt(drandnorm());
                moon->atmosphereRadius = (moon->terrainMaxLevel * 2.0);
                moon->atmosphereAbsorbStrength = 0.7 + 0.3 * drandnorm();
                moon->atmosphereAbsorbColor = glm::vec3(0.1 + drandnorm() * 0.9, 0.1 + drandnorm() * 0.9, 0.1 + drandnorm() * 0.9);
                moon->preferredColor = glm::vec3(drandnorm(), drandnorm(), drandnorm());
            }
            else {
                moon->fluidMaxLevel = 0.0;
                moon->habitableChance = 0.0;
                moon->atmosphereRadius = 0.0;
                moon->atmosphereAbsorbStrength = 0.0;
                moon->atmosphereAbsorbColor = glm::vec3(0.0);
                moon->preferredColor = glm::vec3(drandnorm(), drandnorm(), drandnorm());
            }
            moon->orbitPlane = glm::normalize(glm::vec3(drandnorm(), drandnorm(), drandnorm()) * 2.0f - 1.0f);
            moon->hostDistance = planet->radius * 5.0 * (((float)g) * 0.2 + 1.0);
            moon->orbitSpeed = 1000000.0 * drandnorm() / moon->hostDistance;
            moon->rotationSpeed = drandnorm();
            moon->rotationPlane = glm::normalize(glm::dvec3((drandnorm() * 0.3f) * 2.0f - 1.0f, 1.0f, (drandnorm() * 0.3f) * 2.0f - 1.0f));
            system->bodies.push_back(moon);
        }
    }
    return system;
}
  