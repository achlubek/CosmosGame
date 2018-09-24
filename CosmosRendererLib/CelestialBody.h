#pragma once
#include "AbsCelestialObject.h"

#include "Star.h"

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
    double orbitSpeed; //this should be calculated with star mass and distance from star
    glm::vec3 preferredColor;//0->1
    double atmosphereRadius;
    double atmosphereAbsorbStrength;//0->1
    glm::vec3 atmosphereAbsorbColor;//0->1
    glm::dvec3 orbitPlane;
    glm::dvec3 rotationPlane;
    //TODO define the rotation speed in unit "rotations per full orbit" and make orbit speed dependand on distance from star and mass of star
    // 
    double rotationSpeed;
    AbsCelestialObject* host{ nullptr };
    Star* starhost{ nullptr };
    uint64_t bodyId;

    glm::dvec3 getPosition(double at_time) {
        double orbitLength = 2.0 * glm::pi<double>() * hostDistance * 1000.0;
        double percentagePerSecond = orbitSpeed / orbitLength;
        glm::dvec3 orbitplaneTangent = glm::normalize(glm::cross(orbitPlane, glm::dvec3(0.0, 1.0, 0.0)));

        glm::dquat rotation = glm::angleAxis(percentagePerSecond * at_time * 2.0 * 3.14159265359 * 1.0, glm::dvec3(orbitPlane));
        return host->getPosition(at_time)
            + glm::dvec3(rotation * orbitplaneTangent) * hostDistance;
    }

    glm::mat4 getRotationMatrix(double at_time) {
        //earth rotates 360/24 degree per hour which is = 15 degree
        // seconds in hour 3600, rotation per hour in degree is 15 / 3600
        glm::quat rotationQuat = glm::angleAxis(rotationSpeed * at_time * glm::radians(15.0f / 3600.0f) * 2.0, rotationPlane);
        return glm::mat4_cast(rotationQuat);
    }

    glm::mat4 getFromParentLookAtThisMatrix(double at_time) {
        auto parentPos = starhost->getPosition(at_time);
        auto thisPos = getPosition(at_time);
        auto direction = glm::normalize(thisPos - parentPos);
        return glm::lookAt(glm::dvec3(0.0), -direction, glm::dvec3(0.0, 1.0, 0.0));
    }

    virtual glm::dvec3 getLinearVelocity(double at_time) {
        return getPosition(at_time + 1.0) - getPosition(at_time);
    }

    glm::dvec3 getSurfaceVelocityAtPoint(glm::dvec3 point, double at_time) {
        glm::dvec3 relative = point - getPosition(at_time);
        glm::dvec3 rot1 = glm::dquat(quat_cast(getRotationMatrix(at_time))) * relative;
        glm::dvec3 rot2 = glm::dquat(glm::quat_cast(getRotationMatrix(at_time + 1.0))) * relative;
        return getLinearVelocity(at_time) + (rot1 - rot2);
    }

    CelestialRenderMethod getRenderMethod() {
        if (atmosphereRadius == 0) return CelestialRenderMethod::noAtmosphere;
        else if (atmosphereRadius < 200) return CelestialRenderMethod::lightAtmosphere;
        else return CelestialRenderMethod::thickAtmosphere;
    }
};

