#include "stdafx.h"
#include "AbsCelestialObject.h"


AbsCelestialObject::AbsCelestialObject()
{
}

glm::dvec3 AbsCelestialObject::getGravity(glm::dvec3 at_point, double at_time)
{
    auto objectPos = getPosition(at_time);
    double distance = glm::distance(at_point, objectPos);

    double distanceGM = 0.001 * maxval(radius, distance);
    double density = 5.513;
    double densityMultiplier = density * 0.2793023;
    double radiusGM = radius * 0.001;
    double invradiusGM2 = 1.0 / (distanceGM * distanceGM);
    double volumeGM3 = radiusGM * radiusGM * radiusGM;
    double acceleration = volumeGM3 * invradiusGM2 * densityMultiplier;

    return glm::normalize(objectPos - at_point) * acceleration;
}

double AbsCelestialObject::calculateOrbitVelocity(double altitude)
{
    altitude += radius;
    auto acceleration = glm::length(getGravity(getPosition(0.0) + glm::dvec3(altitude, 0.0, 0.0), 0.0));
    double distanceGM = 0.001 * maxval(radius, altitude);
    return glm::sqrt(distanceGM * acceleration);
}

double AbsCelestialObject::getAltitude(glm::dvec3 at_point, double at_time)
{
    auto objectPos = getPosition(at_time);
    return (glm::distance(at_point, objectPos) - radius);
}

glm::dvec3 AbsCelestialObject::getRelativeLinearVelocity(glm::dvec3 velocity, double at_time)
{
    return getLinearVelocity(at_time) - velocity;
}
 