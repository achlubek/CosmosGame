#include "stdafx.h"
#include "Maneuvering6DOFShipEnginesController.h"
#include "SpaceShipEngine.h"
#include "SpaceShip.h"


Maneuvering6DOFShipEnginesController::Maneuvering6DOFShipEnginesController()
{
}


Maneuvering6DOFShipEnginesController::~Maneuvering6DOFShipEnginesController()
{
}

void Maneuvering6DOFShipEnginesController::update(SpaceShip * ship)
{
    glm::dvec3 angularThrust = (targetAngularVelocity - ship->getAngularVelocity()) * enginesPower * 0.15;
    //X
    negYFORWARD->setPowerPercentage(angularThrust.x);
    negYBACKWARD->setPowerPercentage(-angularThrust.x);
    posYBACKWARD->setPowerPercentage(angularThrust.x);
    posYFORWARD->setPowerPercentage(-angularThrust.x);

    //Z
    negZLEFT->setPowerPercentage(-angularThrust.y);
    negZRIGHT->setPowerPercentage(angularThrust.y);
    posZRIGHT->setPowerPercentage(-angularThrust.y);
    posZLEFT->setPowerPercentage(angularThrust.y);

    //Y
    negXUP->setPowerPercentage(angularThrust.z);
    negXDOWN->setPowerPercentage(-angularThrust.z);
    posXDOWN->setPowerPercentage(angularThrust.z);
    posXUP->setPowerPercentage(-angularThrust.z);

    auto rotmat = glm::mat3_cast(ship->getOrientation());
    glm::dvec3 linearThrust = enginesPower * glm::inverse(rotmat) * ((targetLinearVelocity + referenceFrameVelocity) - ship->getLinearVelocity());

    glm::dvec3 positiveTrust = glm::dvec3(max(0.0, linearThrust.x), max(0.0, linearThrust.y), max(0.0, linearThrust.z));
    glm::dvec3 negativeTrust = -glm::dvec3(min(0.0, linearThrust.x), min(0.0, linearThrust.y), min(0.0, linearThrust.z));
    // TODO CLAMPS NOT NEEDED
    posXUP->setPowerPercentage(glm::clamp(posXUP->getPowerPercentage() + negativeTrust.y, 0.0, 1.0));
    posXDOWN->setPowerPercentage(glm::clamp(posXDOWN->getPowerPercentage() + positiveTrust.y, 0.0, 1.0));
    negXUP->setPowerPercentage(glm::clamp(negXUP->getPowerPercentage() + negativeTrust.y, 0.0, 1.0));
    negXDOWN->setPowerPercentage(glm::clamp(negXDOWN->getPowerPercentage() + positiveTrust.y, 0.0, 1.0));

    posYBACKWARD->setPowerPercentage(glm::clamp(posYBACKWARD->getPowerPercentage() + positiveTrust.z, 0.0, 1.0));
    posYFORWARD->setPowerPercentage(glm::clamp(posYFORWARD->getPowerPercentage() + negativeTrust.z, 0.0, 1.0));
    negYBACKWARD->setPowerPercentage(glm::clamp(negYBACKWARD->getPowerPercentage() + positiveTrust.z, 0.0, 1.0));
    negYFORWARD->setPowerPercentage(glm::clamp(negYFORWARD->getPowerPercentage() + negativeTrust.z, 0.0, 1.0));

    posZLEFT->setPowerPercentage(glm::clamp(posZLEFT->getPowerPercentage() + positiveTrust.x, 0.0, 1.0));
    posZRIGHT->setPowerPercentage(glm::clamp(posZRIGHT->getPowerPercentage() + negativeTrust.x, 0.0, 1.0));
    negZLEFT->setPowerPercentage(glm::clamp(negZLEFT->getPowerPercentage() + positiveTrust.x, 0.0, 1.0));
    negZRIGHT->setPowerPercentage(glm::clamp(negZRIGHT->getPowerPercentage() + negativeTrust.x, 0.0, 1.0));
}
