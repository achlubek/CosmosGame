#pragma once
#include "AbsDrawableComponent.h" 
class BatteryDrainer;
class ThrustGeneratorComponent : public AbsDrawableComponent
{
public:
    ThrustGeneratorComponent(Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation, double maxthrust, double maxwattage);
    ~ThrustGeneratorComponent();    
    
    virtual void update(double elapsed) override;
    virtual void loadFromFile(std::string mediakey) override;
    virtual ThrustGeneratorComponent * clone() override;
    void setPowerPercentage(double p);
private:
    BatteryDrainer * drainer;
    double powerPercentage;
    double maxThrust;
};

