#include "stdafx.h"
#include "SpaceShipHyperDrive.h"

#include "SpaceShip.h"


SpaceShipHyperDrive::SpaceShipHyperDrive(glm::dvec3 relativePosition, glm::dquat relativeOrientation, double power, double imaxWattPower)
    : SpaceShipModule(relativePosition, relativeOrientation, maxWattPower),
    maxPower(power)
{
}


SpaceShipHyperDrive::~SpaceShipHyperDrive()
{
}

void SpaceShipHyperDrive::update(SpaceShip * ship, double time_elapsed)
{
    // trust is negative of applied impulse
    auto m3 = glm::mat3_cast(ship->getOrientation());
    auto m3_thrust = glm::mat3_cast(relativeOrientation);
    auto thrustDirection = m3_thrust * glm::dvec3(0.0, 0.0, -1.0);
    glm::dvec3 vel = -thrustDirection * maxPower * realPower;
    if (realPower < currentPowerPercentage) {
        realPower += min(0.01 * time_elapsed, currentPowerPercentage - realPower);
    }
    else {
        realPower -= min(2.5 * time_elapsed, realPower - currentPowerPercentage);
    }
    realPower = glm::clamp(realPower, 0.0, 1.0);
    ship->setHyperDriveVelocity(m3 * vel);
}