#pragma once
#include "GeneratedPlanetInfo.h"
#include "AbsCelestialObject.h"
class GeneratedMoonInfo : public AbsCelestialObject
{
public:
    GeneratedMoonInfo();
    ~GeneratedMoonInfo();
    double terrainMaxLevel; //0->1
    double fluidMaxLevel; //0->1
    double planetDistance; // real units like sun to earth 149 600 000
    double habitableChance; //0->1
    double orbitSpeed; //0->1
    glm::vec3 preferredColor;//0->1
    double atmosphereRadius;
    double atmosphereAbsorbStrength;//0->1
    glm::vec3 atmosphereAbsorbColor;//0->1
    glm::dvec3 orbitPlane;
    GeneratedPlanetInfo host;
    uint64_t moonId;    

    glm::dvec3 getPosition(double at_time) {
        double time = at_time;
        glm::dvec3 orbitplaneTangent = glm::normalize(glm::cross(orbitPlane, glm::dvec3(0.0, 1.0, 0.0)));
        return host.getPosition(at_time)
            + glm::dvec3(glm::angleAxis(orbitSpeed * time * 0.001, glm::dvec3(orbitPlane)) * glm::dvec3(orbitplaneTangent)) * planetDistance;
    }

    virtual glm::dvec3 getLinearVelocity(double at_time) {
        return getPosition(at_time + 1.0) - getPosition(at_time);
    }

    virtual ObjectType getObjectType() {
        return ObjectType::Moon;
    }
};

