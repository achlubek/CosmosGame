#include "stdafx.h"
#include "ThrustGeneratorComponent.h"


ThrustGeneratorComponent::ThrustGeneratorComponent(Model3d* model, std::string modelName, glm::dvec3 relativePosition, glm::dquat relativeOrientation, 
    double maxthrust, double maxwattage, ParticleSystem* particleSystem, std::string particleSystemName)
    : AbsDrawableComponent(ComponentTypes::ThrustGenerator, model, modelName, relativePosition, relativeOrientation),
    maxThrust(maxthrust), 
    maxWattage(maxwattage),
    drainer(new BatteryDrainer(maxwattage)), 
    particleGenerator(new PointParticlesEmitter(particleSystem)),
    particleSystemName(particleSystemName)
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
    return new ThrustGeneratorComponent(model, modelName, relativePosition, relativeOrientation, maxThrust, drainer->getMaximumWattage(), particleGenerator->getSystem(), particleSystemName);
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

std::string ThrustGeneratorComponent::serialize()
{
    std::stringstream s;
    s << serializeBase();
    s << "modelName=" << modelName << "\n";
    s << "relativePosition=" << relativePosition.x << " " << relativePosition.y << " " << relativePosition.z << "\n";
    s << "relativeOrientation=" << relativeOrientation.w << " " << relativeOrientation.x << " " << relativeOrientation.y << " " << relativeOrientation.z << "\n";
    s << "maxThrust=" << maxThrust << "\n";
    s << "maxWattage=" << maxWattage << "\n";
    s << "particleSystemName=" << particleSystemName << "\n";
    s << "functionalityGroup=" << static_cast<int>(functionalityGroup) << "\n";
    return s.str();
}

ThrustGeneratorComponent * ThrustGeneratorComponent::deserialize(Model3dFactory * model3dFactory, ParticleSystemFactory * particleSystemFactory, std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    auto modelName = reader.gets("modelName");
    auto particleSystemName = reader.gets("particleSystemName");
    auto model = model3dFactory->build(modelName);
    auto particleSystem = particleSystemFactory->build(particleSystemName);
    auto component = new ThrustGeneratorComponent(model, modelName, reader.getdv3("relativePosition"), reader.getdquat("relativeOrientation"), 
        reader.getd("maxThrust"), reader.getd("maxWattage"), particleSystem, particleSystemName);
    component->deserializeBaseInPlace(serializedString);
    component->functionalityGroup = static_cast<ThrustGroup>(reader.geti("functionalityGroup"));
    return component;
}
