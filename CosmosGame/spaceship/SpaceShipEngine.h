#pragma once
#include "SpaceShipModule.h"
class SpaceShipEngine : public SpaceShipModule
{
public:
    SpaceShipEngine(glm::dvec3 relativePosition, glm::dquat relativeOrientation, double power, double maxWattPower);
    ~SpaceShipEngine();
    double maxPower;
    virtual void update(SpaceShip * ship, double time_elapsed) override;
};

