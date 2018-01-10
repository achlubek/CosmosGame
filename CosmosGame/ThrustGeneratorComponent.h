#pragma once
#include "AbsDrawableComponent.h" 
class ThrustGeneratorComponent : public AbsDrawableComponent
{
public:
    ThrustGeneratorComponent(Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation);
    ~ThrustGeneratorComponent();    
    
    virtual void update(double elapsed) override;
    virtual void loadFromFile(std::string mediakey) override;
    virtual ThrustGeneratorComponent * clone() override;
};

