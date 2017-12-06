#include "stdafx.h"
#include "SpaceShipEngine.h"
#include "SpaceShip.h"


SpaceShipEngine::SpaceShipEngine(glm::dvec3 relativePosition, glm::dquat relativeOrientation, double power, double maxWattPower)
    : SpaceShipModule(relativePosition, relativeOrientation, maxWattPower),
    maxPower(power)
{
}

SpaceShipEngine::~SpaceShipEngine()
{
}

void SpaceShipEngine::update(SpaceShip * ship, double time_elapsed)
{
    // trust is negative of applied impulse
    auto m3_thrust = glm::mat3_cast(relativeOrientation);
    auto thrustDirection = m3_thrust * glm::dvec3(0.0, 0.0, 1.0);
    glm::dvec3 force = -thrustDirection * maxPower * currentPowerPercentage;
    ship->applyImpulse(relativePosition, force);
}
 