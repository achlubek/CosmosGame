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


void ShipEnginesUnitedController::addEnginesLinearPositiveX(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearPositiveX.insert(std::end(enginesLinearPositiveX), std::begin(engines), std::end(engines)); 
}

void ShipEnginesUnitedController::addEnginesLinearNegativeX(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearPositiveX.insert(std::end(enginesLinearPositiveX), std::begin(engines), std::end(engines));
}


void ShipEnginesUnitedController::addEnginesLinearPositiveY(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearPositiveY.insert(std::end(enginesLinearPositiveY), std::begin(engines), std::end(engines));
}

void ShipEnginesUnitedController::addEnginesLinearNegativeY(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearNegativeY.insert(std::end(enginesLinearNegativeY), std::begin(engines), std::end(engines));
}


void ShipEnginesUnitedController::addEnginesLinearPositiveZ(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearPositiveZ.insert(std::end(enginesLinearPositiveZ), std::begin(engines), std::end(engines));
}

void ShipEnginesUnitedController::addEnginesLinearNegativeZ(std::vector<SpaceShipEngine*> engines)
{
    enginesLinearNegativeZ.insert(std::end(enginesLinearNegativeZ), std::begin(engines), std::end(engines));
}



void ShipEnginesUnitedController::addEnginesAngularPositiveX(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularPositiveX.insert(std::end(enginesAngularPositiveX), std::begin(engines), std::end(engines));
}

void ShipEnginesUnitedController::addEnginesAngularNegativeX(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularNegativeX.insert(std::end(enginesAngularNegativeX), std::begin(engines), std::end(engines));
}


void ShipEnginesUnitedController::addEnginesAngularPositiveY(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularPositiveY.insert(std::end(enginesAngularPositiveY), std::begin(engines), std::end(engines));
}

void ShipEnginesUnitedController::addEnginesAngularNegativeY(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularNegativeY.insert(std::end(enginesAngularNegativeY), std::begin(engines), std::end(engines));
}


void ShipEnginesUnitedController::addEnginesAngularPositiveZ(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularPositiveZ.insert(std::end(enginesAngularPositiveZ), std::begin(engines), std::end(engines));
}

void ShipEnginesUnitedController::addEnginesAngularNegativeZ(std::vector<SpaceShipEngine*> engines)
{
    enginesAngularNegativeZ.insert(std::end(enginesAngularNegativeZ), std::begin(engines), std::end(engines));
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
