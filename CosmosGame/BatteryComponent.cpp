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

void BatteryComponent::loadFromFile(std::string mediakey)
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
