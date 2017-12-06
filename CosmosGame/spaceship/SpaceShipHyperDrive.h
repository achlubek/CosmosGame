#pragma once
#include "SpaceShipModule.h"
class SpaceShipHyperDrive : public SpaceShipModule
{
public:
    SpaceShipHyperDrive(glm::dvec3 relativePosition, glm::dquat relativeOrientation, double power, double imaxWattPower);
    ~SpaceShipHyperDrive();
    double maxPower;

    virtual void update(SpaceShip * ship, double time_elapsed) override;
private:
    double realPower = 0.0;
};

