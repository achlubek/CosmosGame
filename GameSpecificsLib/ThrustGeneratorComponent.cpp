#include "stdafx.h"
#include "ThrustGeneratorComponent.h"
#include "BatteryDrainer.h"
#include "GameObject.h"
#include "Transformation3DComponent.h"
#include "PointParticlesEmitter.h"


ThrustGeneratorComponent::ThrustGeneratorComponent(Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation, 
    double maxthrust, double maxwattage, ParticleSystem* particleSystem)
    : AbsDrawableComponent(ComponentTypes::ThrustGenerator, model, relativePosition, relativeOrientation), 
    maxThrust(maxthrust), 
    drainer(new BatteryDrainer(maxwattage)), 
    particleGenerator(new PointParticlesEmitter(particleSystem))
{
    
}


ThrustGeneratorComponent::~ThrustGeneratorComponent()
{
}

void ThrustGeneratorComponent::update(double elapsed)
{
    Transformation3DComponent* transform = owner->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);

    drainer->setOwner(owner);
    double realPower = drainer->extractEnergy(elapsed, powerPercentage) * powerPercentage;
    auto thrustDirection = getThrustVector();
    glm::dvec3 force = -thrustDirection * maxThrust * realPower;

    particleGenerator->updateProperties(transform->getPosition() + getWorldTranslation(), transform->getLinearVelocity(), glm::mat3_cast(transform->getOrientation()) * thrustDirection, 10.0 + realPower * 50.0, 0.6, 0.2);
    particleGenerator->setParticlesPerSecond(realPower * 100.0);
    particleGenerator->update(elapsed);

    transform->applyImpulse(relativePosition, force);
}

ThrustGeneratorComponent * ThrustGeneratorComponent::clone()
{
    return new ThrustGeneratorComponent(model, relativePosition, relativeOrientation, maxThrust, drainer->getMaximumWattage(), particleGenerator->getSystem());
}

void ThrustGeneratorComponent::setPowerPercentage(double p)
{
    powerPercentage = glm::clamp(p, 0.0, 1.0);
}

double ThrustGeneratorComponent::getPowerPercentage()
{
    return powerPercentage;
}

glm::dvec3 ThrustGeneratorComponent::getThrustVector()
{
    auto m3_thrust = glm::mat3_cast(relativeOrientation);
    auto thrustDirection = m3_thrust * glm::dvec3(0.0, 0.0, 1.0);
    return thrustDirection;
}
