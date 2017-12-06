#pragma once
class SpaceShip;
class Model3d;
class SpaceShipModule
{
public:
    SpaceShipModule(Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation, double maxWattPower);
    ~SpaceShipModule();
    double getCurrentWattPower();
    virtual void update(SpaceShip* ship, double time_elapsed) = 0;
    bool isEnabled();
    void enable();
    void disable();
    void setEnabled(bool value);
    void setRelativePosition(glm::dvec3 pos);
    glm::dvec3 getRelativePosition();
    void setRelativeOrientation(glm::dquat orient);
    glm::dquat getRelativeOrientation();
    void setPowerPercentage(double percent);
    double getPowerPercentage();
    Model3d* model;
protected:
    double maxWattPower;
    double currentPowerPercentage = 0.0;
    glm::dvec3 relativePosition;
    glm::dquat relativeOrientation;
    bool enabled;
};

