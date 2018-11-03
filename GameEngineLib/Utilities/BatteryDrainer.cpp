#include "stdafx.h"
#include "BatteryDrainer.h"


BatteryDrainer::BatteryDrainer(GameObject* iowner, double imaximumWattage)
    : owner(iowner), maximumWattage(imaximumWattage)
{
}

BatteryDrainer::BatteryDrainer(double imaximumWattage)
    : maximumWattage(imaximumWattage)
{
}


BatteryDrainer::~BatteryDrainer()
{
}

double BatteryDrainer::extractEnergy(double elapsed, double powerPercentage)
{
    if (powerPercentage == 0.0) return 1.0;
    if (maximumWattage == 0.0) return 1.0;
    BatteryComponent* battery = owner->getComponent<BatteryComponent>(ComponentTypes::Battery);
    double energy = battery->getEnergy();
    double output = 1.0;
    if (energy < elapsed * powerPercentage * maximumWattage) {
        output = energy / (elapsed * powerPercentage * maximumWattage);
    }
    battery->discharge(elapsed, powerPercentage * maximumWattage);
    return output;
}

BatteryDrainer * BatteryDrainer::clone()
{
    return new BatteryDrainer(owner, maximumWattage);
}

void BatteryDrainer::setOwner(GameObject * iowner)
{
    owner = iowner;
}

double BatteryDrainer::getMaximumWattage()
{
    return maximumWattage;
}

GameObject * BatteryDrainer::getOwner()
{
    return owner;
}
