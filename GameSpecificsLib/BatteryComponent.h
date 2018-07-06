#pragma once
#include "AbsComponent.h"
#include "ComponentTypes.h"
class BatteryComponent : public AbsComponent
{
public:
    BatteryComponent(double maximumEnergy, double energyAmount);
    ~BatteryComponent();
     
    virtual void update(double elapsed) override;
    virtual AbsComponent * clone() override;
    double getEnergy();
    void discharge(double elapsed, double amount);
    void charge(double elapsed, double amount);
private:
    double energy;
    double maxEnergy;
};

