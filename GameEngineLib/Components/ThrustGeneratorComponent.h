#pragma once
#include "AbsDrawableComponent.h" 
class PointParticlesEmitter;
class BatteryDrainer;
class ParticleSystem;
class ParticleSystemFactory;
class Model3dFactory;
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
    ThrustGeneratorComponent(Model3d* model, std::string modelName, glm::dvec3 relativePosition, glm::dquat relativeOrientation, double maxthrust, double maxwattage, ParticleSystem* particleSystem, std::string particleSystemName);
    ~ThrustGeneratorComponent();    
    
    virtual void update(double elapsed) override;
    virtual ThrustGeneratorComponent * clone() override;
    void setPowerPercentage(double p);
    double getPowerPercentage();
    glm::dvec3 getThrustVector();
    ThrustGroup functionalityGroup;

    virtual std::string serialize() override;
    static ThrustGeneratorComponent* deserialize(Model3dFactory* model3dFactory, ParticleSystemFactory* particleSystemFactory, std::string serializedString);

private:
    BatteryDrainer * drainer;
    PointParticlesEmitter* particleGenerator;
    std::string particleSystemName;
    double powerPercentage{ 0 };
    double maxThrust;
    double maxWattage;
};
