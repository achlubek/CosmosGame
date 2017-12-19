#include "stdafx.h"
#include "SpaceShipEngine.h"
#include "SpaceShip.h"
#include "../Model3d.h"


SpaceShipEngine::SpaceShipEngine(Model3d* model, std::string name, glm::dvec3 relativePosition, glm::dquat relativeOrientation, double power, double maxWattPower)
    : SpaceShipModule(model, name, relativePosition, relativeOrientation, maxWattPower),
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

int SpaceShipEngine::getType()
{
    return 2;
}
 