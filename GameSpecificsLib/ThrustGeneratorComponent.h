#pragma once
#include "AbsDrawableComponent.h" 
class PointParticlesEmitter;
class BatteryDrainer;
class ParticleSystem;
enum ThrustGroup {
    forward = 0,
    backward = 1,
    left = 2,
    right = 3,
    up = 4,
    down = 5,

    pitch_up = 6,
    pitch_down = 7,
    roll_left = 8,
    roll_right = 9,
    yaw_left = 10,
    yaw_right = 11
};
class ThrustGeneratorComponent : public AbsDrawableComponent
{
public:
    ThrustGeneratorComponent(Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation, double maxthrust, double maxwattage, ParticleSystem* particleSystem);
    ~ThrustGeneratorComponent();    
    
    virtual void update(double elapsed) override;
    virtual void loadFromFile(std::string mediakey) override;
    virtual ThrustGeneratorComponent * clone() override;
    void setPowerPercentage(double p);
    double getPowerPercentage();
    glm::dvec3 getThrustVector();
    ThrustGroup functionalityGroup;
private:
    BatteryDrainer * drainer;
    PointParticlesEmitter* particleGenerator;
    double powerPercentage{ 0 };
    double maxThrust;
};

