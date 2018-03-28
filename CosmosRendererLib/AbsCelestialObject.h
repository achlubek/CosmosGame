#pragma once
#define maxval(a,b) (a>b?a:b)
class AbsCelestialObject
{
public:
    AbsCelestialObject();
    virtual ~AbsCelestialObject() {}

    double radius;
    double mass;

    glm::dvec3 getGravity(glm::dvec3 at_point, double at_time) {
        auto objectPos = getPosition(at_time);
        double distance = glm::distance(at_point, objectPos);
        double G = 0.00006674;
        double strength = (G * (mass / maxval(radius, distance * distance))) * 0.000001; // the max is here to avoid infinities around 0 distance okay
        return glm::normalize(objectPos - at_point) * strength;
    }

    virtual glm::dvec3 getLinearVelocity(double at_time) = 0;
    virtual glm::dvec3 getPosition(double at_time) = 0;
};

