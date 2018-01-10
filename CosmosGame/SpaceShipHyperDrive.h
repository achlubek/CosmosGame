#pragma once
#include "SpaceShipModule.h"
class SpaceShipHyperDrive : public SpaceShipModule
{
public:
    SpaceShipHyperDrive(Model3d* model, std::string name, glm::dvec3 relativePosition, glm::dquat relativeOrientation, double power, double imaxWattPower);
    ~SpaceShipHyperDrive();
    double maxPower;

    virtual void update(SpaceShip * ship, double time_elapsed) override;
    virtual int getType() override;
private:
    double realPower = 0.0;
};

