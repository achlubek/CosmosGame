#pragma once
#include "AbsComponent.h"
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

    virtual std::string serialize() override;
    static BatteryComponent* deserialize(std::string serializedString);
private:
    double energy;
    double maxEnergy;
};

