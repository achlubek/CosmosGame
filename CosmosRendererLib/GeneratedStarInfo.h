#pragma once
#include "AbsCelestialObject.h"
class GeneratedStarInfo : public AbsCelestialObject
{
public:
    GeneratedStarInfo();
    ~GeneratedStarInfo();

    int64_t x;
    int64_t y;
    int64_t z;
    uint64_t seed;
    glm::dvec3 color; //0->maybe 10? maybe 100?
    double age; //0->1
    double spotsIntensity; //0->1
    double rotationSpeed; // 0-> 1
    glm::dvec3 orbitPlane; // normalized direction, normalize(cross(orbitPlane, up_vector)) == planet line
    uint8_t planetsCount;
    uint64_t starId;
    std::vector<size_t> idsOfPlanets = {};

    virtual glm::dvec3 getPosition(double at_time) {
        return glm::dvec3(x, y, z);
    }

    virtual glm::dvec3 getLinearVelocity(double at_time) {
        return glm::dvec3(0.0);
    }
    virtual ObjectType getObjectType() {
        return ObjectType::Star;
    }
};

