#include "stdafx.h"
#include "SpaceShipModule.h"


SpaceShipModule::SpaceShipModule(glm::dvec3 irelativePosition, glm::dquat irelativeOrientation, double imaxWattPower)
    : relativePosition (irelativePosition),
    maxWattPower(imaxWattPower),
    relativeOrientation(irelativeOrientation)
{
    enabled = false;
}


SpaceShipModule::~SpaceShipModule()
{
}

bool SpaceShipModule::isEnabled()
{
    return enabled;
}

void SpaceShipModule::enable()
{
    enabled = true;
}

void SpaceShipModule::disable()
{
    enabled = false;
}

void SpaceShipModule::setEnabled(bool value)
{
    enabled = value;
}

void SpaceShipModule::setRelativePosition(glm::dvec3 pos)
{
    relativePosition = pos;
}

glm::dvec3 SpaceShipModule::getRelativePosition()
{
    return relativePosition;
}

void SpaceShipModule::setRelativeOrientation(glm::dquat orient)
{
    relativeOrientation = orient;
}

glm::dquat SpaceShipModule::getRelativeOrientation()
{
    return relativeOrientation;
}

void SpaceShipModule::setPowerPercentage(double percent)
{
    if (percent < 0.0) percent = 0.0;
    if (percent > 1.0) percent = 1.0;
    currentPowerPercentage = percent;
}

double SpaceShipModule::getCurrentWattPower()
{
    return currentPowerPercentage * maxWattPower;
}

double SpaceShipModule::getPowerPercentage()
{
    return currentPowerPercentage;
}
