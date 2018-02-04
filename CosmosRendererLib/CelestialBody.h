#pragma once
#include "AbsCelestialObject.h"
enum CelestialRenderMethod {
    noAtmosphere = 1,
    lightAtmosphere = 2,
    thickAtmosphere = 3
};
class CelestialBody : public AbsCelestialObject
{
public:
    CelestialBody();
    ~CelestialBody();
    double terrainMaxLevel; //0->1
    double fluidMaxLevel; //0->1
    double hostDistance; // real units like sun to earth 149 600 000
    double habitableChance; //0->1
    double orbitSpeed; //0->1
    glm::vec3 preferredColor;//0->1
    double atmosphereRadius;
    double atmosphereAbsorbStrength;//0->1
    glm::vec3 atmosphereAbsorbColor;//0->1
    glm::dvec3 orbitPlane;
    AbsCelestialObject* host{ nullptr };
    uint64_t bodyId;

    glm::dvec3 getPosition(double at_time) {
        double time = at_time;
        glm::dvec3 orbitplaneTangent = glm::normalize(glm::cross(orbitPlane, glm::dvec3(0.0, 1.0, 0.0)));
        return host->getPosition(at_time)
            + glm::dvec3(glm::angleAxis(orbitSpeed * time * 0.001, glm::dvec3(orbitPlane)) * glm::dvec3(orbitplaneTangent)) * hostDistance;
    }

    virtual glm::dvec3 getLinearVelocity(double at_time) {
        return getPosition(at_time + 1.0) - getPosition(at_time);
    }

    CelestialRenderMethod getRenderMethod() {
        if (atmosphereRadius == 0) return CelestialRenderMethod::noAtmosphere;
        else if (atmosphereRadius < 200) return CelestialRenderMethod::lightAtmosphere;
        else return CelestialRenderMethod::thickAtmosphere;
    }
};
