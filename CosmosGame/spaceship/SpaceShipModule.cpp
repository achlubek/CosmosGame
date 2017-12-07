#include "stdafx.h"
#include "SpaceShipModule.h"
#include "SpaceShipEngine.h"
#include "SpaceShipHyperDrive.h"
#include "INIReader.h";
#include "../Model3d.h"
#include "../CosmosRenderer.h"


SpaceShipModule::SpaceShipModule(Model3d* imodel, glm::dvec3 irelativePosition, glm::dquat irelativeOrientation, double imaxWattPower)
    : relativePosition (irelativePosition),
    maxWattPower(imaxWattPower),
    model(imodel),
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

SpaceShipEngine* SpaceShipModule::loadSpaceShipEngine(string mediakey)
{
    INIReader reader = INIReader(mediakey);
    Model3d* model = new Model3d(CosmosRenderer::instance->vulkan, CosmosRenderer::instance->modelMRTLayout, reader.gets("model"));
    double power = reader.getf("power");
    double watt_power = reader.getf("watt_power");
    return new SpaceShipEngine(model, glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0), power, watt_power);
}

SpaceShipHyperDrive* SpaceShipModule::loadSpaceShipHyperDrive(string mediakey)
{
    INIReader reader = INIReader(mediakey);
    Model3d* model = new Model3d(CosmosRenderer::instance->vulkan, CosmosRenderer::instance->modelMRTLayout, reader.gets("model"));
    double power = reader.getf("power");
    double watt_power = reader.getf("watt_power");
    return new SpaceShipHyperDrive(model, glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0), power, watt_power);
}
