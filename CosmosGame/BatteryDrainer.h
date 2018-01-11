#pragma once
class GameObject;
class BatteryDrainer
{
public:
    BatteryDrainer(GameObject* owner, float maximumWattage);
    BatteryDrainer(float maximumWattage);
    ~BatteryDrainer();
    // it returns 0.0->1.0 , 1.0 if everything is ok and theres energy to do the task 0.0 if theres no energy at all so device shouldnt run
    double extractEnergy(double elapsed, double powerPercentage);
    BatteryDrainer* clone();
    void setOwner(GameObject* owner);
    double getMaximumWattage();
    GameObject* getOwner();
private:
    GameObject * owner = nullptr;
    double maximumWattage;
};

