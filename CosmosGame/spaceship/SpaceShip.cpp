#include "stdafx.h"
#include "SpaceShip.h"
#include "SpaceShipModule.h"
#include "Object3dInfo.h"
#include "../physics/PhysicalEntity.h"
#include "../Model3d.h"
//#define GLM_ENABLE_EXPERIMENTAL
//#include "glm\gtx\intersect.hpp"


SpaceShip::SpaceShip(Object3dInfo* collisionShape, Model3d* imodel, glm::dvec3 pos, glm::dquat orient)
    : PhysicalEntity(collisionShape, 1000.0, pos, orient), modules({}), model(imodel)
{

}


SpaceShip::~SpaceShip()
{
}


void SpaceShip::setHyperDriveVelocity(glm::dvec3 vel)
{
    hyperDriveVelocity = vel;
}

void SpaceShip::drawShipAndModules(VulkanRenderStage * stage, VulkanDescriptorSet* celestialSet, glm::dvec3 observerPosition)
{
    model->draw(stage, celestialSet, getPosition() - observerPosition, getOrientation());
    auto m3_shiprot = glm::mat3_cast(getOrientation());
    for (int i = 0; i < modules.size(); i++) {
        auto m3_shiprot = glm::mat3_cast(getOrientation());
        modules[i]->model->draw(stage, celestialSet, m3_shiprot * modules[i]->getRelativePosition(), getOrientation() * modules[i]->getRelativeOrientation());
    }
}

void SpaceShip::update(double time_elapsed)
{
    for (int i = 0; i < modules.size(); i++) {
        if (modules[i]->isEnabled()) {
            modules[i]->update(this, time_elapsed);
        }
    }
    if (mainSeat != nullptr) {
        mainSeat->setPosition(getPosition() + getOrientation() * mainSeatPosition);
        mainSeat->setOrientation(mainSeatOrientation * getOrientation());
        mainSeat->setLinearVelocity(getLinearVelocity());
        mainSeat->setAngularVelocity(getAngularVelocity());
    }
}
