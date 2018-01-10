#pragma once 
class ShipEnginesUnitedController;
class SpaceShipModule;
class Object3dInfo;
class Model3d;
class VulkanRenderStage;
#include "PhysicalEntity.h"


class SpaceShip : public PhysicalEntity
{
public:
    SpaceShip(Model3d* model, glm::dvec3 pos, glm::dquat orient);
    ~SpaceShip();
    
    void setHyperDriveVelocity(glm::dvec3 vel);
    std::vector<SpaceShipModule*> modules;
    ShipEnginesUnitedController* unitedController;
    PhysicalEntity* mainSeat = nullptr;
    Model3d* model;
    glm::dvec3 mainSeatPosition = glm::dvec3(0.0, 0.6, 12.5);
    glm::dquat mainSeatOrientation = glm::dquat(1.0, 0.0, 0.0, 0.0);
    void drawShipAndModules(VulkanRenderStage* stage, VulkanDescriptorSet* celestialSet, glm::dvec3 observerPosition);
private:

    glm::dvec3 hyperDriveVelocity;
    
    virtual void update(double time_elapsed) override;
};

