#include "stdafx.h"
#include "BatteryComponent.h"


BatteryComponent::BatteryComponent(double maximumEnergy, double energyAmount)
    : AbsComponent(ComponentTypes::Battery), energy(energyAmount), maxEnergy(maximumEnergy)
{
}


BatteryComponent::~BatteryComponent()
{
}

void BatteryComponent::update(double elapsed)
{
}

AbsComponent * BatteryComponent::clone()
{
    return new BatteryComponent(maxEnergy, energy);
}

double BatteryComponent::getEnergy()
{
    return energy;
}

void BatteryComponent::discharge(double elapsed, double amount)
{
    energy -= elapsed * amount;
    energy = glm::clamp(energy, 0.0, maxEnergy);
}

void BatteryComponent::charge(double elapsed, double amount)
{
    energy += elapsed * amount;
    energy = glm::clamp(energy, 0.0, maxEnergy);
}

std::string BatteryComponent::serialize()
{
    std::stringstream s;
    s << serializeBase();
    s << "energy=" << energy << "\n";
    s << "maxEnergy=" << maxEnergy << "\n";
    return s.str();
}

BatteryComponent * BatteryComponent::deserialize(std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    auto component = new BatteryComponent(
        reader.getd("maxEnergy"),
        reader.getd("energy")
    );
    component->deserializeBaseInPlace(serializedString);
    return component;
}
