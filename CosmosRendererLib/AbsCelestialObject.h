#pragma once
#define maxval(a,b) (a>b?a:b)
class AbsCelestialObject
{
public:
    AbsCelestialObject();
    virtual ~AbsCelestialObject() {}

    double radius; 

    glm::dvec3 getGravity(glm::dvec3 at_point, double at_time) {/*
        double G = 0.00000000006674;
        double strength = (G * (calculateMass(radius) / maxval(radius * radius, distance * distance))); // the max is here to avoid infinities around 0 distance okay
        return glm::normalize(objectPos - at_point) * strength;*/
        auto objectPos = getPosition(at_time);
        double distance = glm::distance(at_point, objectPos);
        double distanceGM = 0.001 * maxval(radius * 10.0, distance);
        double density = 5.513;
        double densityMultiplier = density / 5.513;
        double radiusGM = radius * 0.001 * 10.0;
        double invradiusGM2 = 1.0 / (distanceGM * distanceGM);
        double volumeGM3 = radiusGM * radiusGM * radiusGM;
        double magicMultiplier = 1.53979;
        return glm::normalize(objectPos - at_point) * magicMultiplier * volumeGM3 * invradiusGM2 * densityMultiplier;
    }

    double calculateOrbitVelocity(double distance) {
        distance += radius;
        auto acceleration = glm::length(getGravity(getPosition(0.0) + glm::dvec3(distance, 0.0, 0.0), 0.0));
        double distanceGM = 0.001 * maxval(radius, distance) * 10.0;
        printf("%f\n", distanceGM * acceleration);
        return distanceGM * acceleration;
    }

    double getAltitude(glm::dvec3 at_point, double at_time) {
        auto objectPos = getPosition(at_time);
        return glm::distance(at_point, objectPos) - radius;
    }


    glm::dvec3 getRelativeLinearVelocity(glm::dvec3 velocity, double at_time) {
        return velocity - getLinearVelocity(at_time);
    }

    virtual glm::dvec3 getLinearVelocity(double at_time) = 0;
    virtual glm::dvec3 getPosition(double at_time) = 0;
};

