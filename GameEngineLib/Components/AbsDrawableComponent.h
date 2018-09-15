#pragma once
#include "AbsComponent.h"
class Model3d;
class AbsDrawableComponent : public AbsComponent
{
public:
    AbsDrawableComponent(Model3d* model, std::string modelName, glm::dvec3 relativePosition, glm::dquat relativeOrientation);
    AbsDrawableComponent(ComponentTypes type, Model3d* model, std::string modelName, glm::dvec3 relativePosition, glm::dquat relativeOrientation);
    virtual ~AbsDrawableComponent() {};
    virtual bool isDrawable() override;
    void draw(glm::dvec3 observerPosition, VulkanRenderStage* stage, VulkanDescriptorSet* set, double scale);
    glm::dvec3 getWorldTranslation();
    virtual void update(double elapsed) override;
    virtual AbsDrawableComponent * clone() override;

    void setRelativePosition(glm::dvec3 relativePosition);
    glm::dvec3 getRelativePosition();
    void setRelativeOrientation(glm::dquat relativeOrientation);
    glm::dquat getRelativeOrientation();    
    
    virtual std::string serialize() override;
    static AbsDrawableComponent* deserialize(Model3dFactory* model3dFactory, std::string serializedString);

protected:
    glm::dvec3 relativePosition;
    glm::dquat relativeOrientation;
    Model3d* model;
    std::string modelName;
    float emissionValue = 0.0;
};

