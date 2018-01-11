#include "stdafx.h"
#include "ThrustGeneratorComponent.h"
#include "BatteryDrainer.h"
#include "GameObject.h"
#include "Transformation3DComponent.h"


ThrustGeneratorComponent::ThrustGeneratorComponent(Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation, double maxthrust, double maxwattage)
    : AbsDrawableComponent(ComponentTypes::ThrustGenerator, model, relativePosition, relativeOrientation), maxThrust(maxthrust), drainer(new BatteryDrainer(maxwattage))
{
}


ThrustGeneratorComponent::~ThrustGeneratorComponent()
{
}

void ThrustGeneratorComponent::update(double elapsed)
{
    double realPower = drainer->extractEnergy(elapsed, powerPercentage) * powerPercentage;
    Transformation3DComponent* transform = owner->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);

    auto m3_thrust = glm::mat3_cast(relativeOrientation);
    auto thrustDirection = m3_thrust * glm::dvec3(0.0, 0.0, 1.0);
    glm::dvec3 force = -thrustDirection * maxThrust * realPower;
    transform->applyImpulse(relativePosition, force);
}

void ThrustGeneratorComponent::loadFromFile(std::string mediakey)
{
}

ThrustGeneratorComponent * ThrustGeneratorComponent::clone()
{
    return new ThrustGeneratorComponent(model, relativePosition, relativeOrientation, maxThrust, drainer->getMaximumWattage());
}

void ThrustGeneratorComponent::setPowerPercentage(double p)
{
}
