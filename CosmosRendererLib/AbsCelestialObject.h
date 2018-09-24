#pragma once

#define maxval(a,b) (a>b?a:b)

class AbsCelestialObject
{
public:
    AbsCelestialObject();
    virtual ~AbsCelestialObject() {}

    double radius; 

    virtual glm::dvec3 getGravity(glm::dvec3 at_point, double at_time);

    double calculateOrbitVelocity(double altitude);

    double getAltitude(glm::dvec3 at_point, double at_time);

    glm::mat4 getFromThisLookAtPointMatrix(double at_time, glm::dvec3 point) { // todo tidy up this mess
        auto pos = getPosition(at_time);
        auto direction = glm::normalize(point - pos);
        return glm::lookAt(glm::dvec3(0.0), -direction, glm::dvec3(0.0, 1.0, 0.0));
    }

    glm::dvec3 getRelativeLinearVelocity(glm::dvec3 velocity, double at_time);

    virtual glm::dvec3 getLinearVelocity(double at_time) = 0;
    virtual glm::dvec3 getPosition(double at_time) = 0;
};

