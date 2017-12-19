#include "stdafx.h"
#include "ShipEnginesUnitedController.h"


ShipEnginesUnitedController::ShipEnginesUnitedController()
    : // yay long init list
    enginesLinearPositiveX({}),
    enginesLinearNegativeX({}),

    enginesLinearPositiveY({}),
    enginesLinearNegativeY({}),

    enginesLinearPositiveZ({}),
    enginesLinearNegativeZ({}),

    enginesAngularPositiveX({}),
    enginesAngularNegativeX({}),

    enginesAngularPositiveY({}),
    enginesAngularNegativeY({}),

    enginesAngularPositiveZ({}),
    enginesAngularNegativeZ({})
{
}


ShipEnginesUnitedController::~ShipEnginesUnitedController()
{
}

void ShipEnginesUnitedController::setLinearThrust(glm::dvec3 lt)
{
    linearThrust = lt;
}

void ShipEnginesUnitedController::setAngularThrust(glm::dvec3 at)
{
    angularThrust = at;
}


void ShipEnginesUnitedController::setEnginesLinearPositiveX(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearPositiveX = engines;
}

void ShipEnginesUnitedController::setEnginesLinearNegativeX(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearNegativeX = engines;
}


void ShipEnginesUnitedController::setEnginesLinearPositiveY(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearPositiveY = engines;
}

void ShipEnginesUnitedController::setEnginesLinearNegativeY(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearNegativeY = engines;
}


void ShipEnginesUnitedController::setEnginesLinearPositiveZ(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearPositiveZ = engines;
}

void ShipEnginesUnitedController::setEnginesLinearNegativeZ(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearNegativeZ = engines;
}



void ShipEnginesUnitedController::setEnginesAngularPositiveX(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularPositiveX = engines;
}

void ShipEnginesUnitedController::setEnginesAngularNegativeX(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularNegativeX = engines;
}


void ShipEnginesUnitedController::setEnginesAngularPositiveY(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularPositiveY = engines;
}

void ShipEnginesUnitedController::setEnginesAngularNegativeY(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularNegativeY = engines;
}


void ShipEnginesUnitedController::setEnginesAngularPositiveZ(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularPositiveZ = engines;
}

void ShipEnginesUnitedController::setEnginesAngularNegativeZ(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularNegativeZ = engines;
}

void ShipEnginesUnitedController::updateEnginesThrust()
{
    // ship modules precentages are clamped on being set, to it makes this class simplier
    // linear thrust:
    setAllEnginesInVectorPower(enginesLinearPositiveX, linearThrust.x);
    setAllEnginesInVectorPower(enginesLinearNegativeX, -linearThrust.x);

    setAllEnginesInVectorPower(enginesLinearPositiveY, linearThrust.y);
    setAllEnginesInVectorPower(enginesLinearNegativeY, -linearThrust.y);

    setAllEnginesInVectorPower(enginesLinearPositiveZ, linearThrust.z);
    setAllEnginesInVectorPower(enginesLinearNegativeZ, -linearThrust.z);

    // angular thrust:
    setAllEnginesInVectorPower(enginesAngularPositiveX, angularThrust.x);
    setAllEnginesInVectorPower(enginesAngularNegativeX, -angularThrust.x);

    setAllEnginesInVectorPower(enginesAngularPositiveY, angularThrust.y);
    setAllEnginesInVectorPower(enginesAngularNegativeY, -angularThrust.y);

    setAllEnginesInVectorPower(enginesAngularPositiveZ, angularThrust.z);
    setAllEnginesInVectorPower(enginesAngularNegativeZ, -angularThrust.z);
}

void ShipEnginesUnitedController::setAllEnginesInVectorPower(std::vector<SpaceShipEngine*> &engines, double power)
{
    for (int i = 0; i < engines.size(); i++) {
        engines[i]->setPowerPercentage(power);
    }
}
