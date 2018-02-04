#pragma once
#include "AbsComponent.h"
class Model3d;
class AbsDrawableComponent : public AbsComponent
{
public:
    AbsDrawableComponent(Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation);
    AbsDrawableComponent(ComponentTypes type, Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation);
    virtual ~AbsDrawableComponent() {};
    virtual bool isDrawable() override;
    void draw(glm::dvec3 observerPosition, VulkanRenderStage* stage, VulkanDescriptorSet* set);
    virtual void update(double elapsed) override;
    virtual void loadFromFile(std::string mediakey) override;
    virtual AbsDrawableComponent * clone() override;
protected:
    glm::dvec3 relativePosition;
    glm::dquat relativeOrientation;
    Model3d* model;

    // Odziedziczono za poœrednictwem elementu AbsComponent
};

