#pragma once
#include "SpaceShipEngine.h"
enum ShipEnginesUnitedControllerFunctionalities {
    forward = 0,
    backward = 1,
    left = 2,
    right = 3,
    up = 4,
    down = 5,

    pitch_up = 6,
    pitch_down = 7,
    roll_left = 8,
    roll_right = 9,
    yaw_left = 10,
    yaw_right = 11
};
class ShipEnginesUnitedController
{
public:
    ShipEnginesUnitedController();
    ~ShipEnginesUnitedController();

    void setLinearThrust(glm::dvec3 lt);
    void setAngularThrust(glm::dvec3 at);

    void setEnginesLinearPositiveX(std::vector<SpaceShipEngine*> engines);
    void setEnginesLinearNegativeX(std::vector<SpaceShipEngine*> engines);
    void setEnginesLinearPositiveY(std::vector<SpaceShipEngine*> engines);
    void setEnginesLinearNegativeY(std::vector<SpaceShipEngine*> engines);
    void setEnginesLinearPositiveZ(std::vector<SpaceShipEngine*> engines);
    void setEnginesLinearNegativeZ(std::vector<SpaceShipEngine*> engines);
    void setEnginesAngularPositiveX(std::vector<SpaceShipEngine*> engines);
    void setEnginesAngularNegativeX(std::vector<SpaceShipEngine*> engines);
    void setEnginesAngularPositiveY(std::vector<SpaceShipEngine*> engines);
    void setEnginesAngularNegativeY(std::vector<SpaceShipEngine*> engines);
    void setEnginesAngularPositiveZ(std::vector<SpaceShipEngine*> engines);
    void setEnginesAngularNegativeZ(std::vector<SpaceShipEngine*> engines);

    void addEnginesLinearPositiveX(std::vector<SpaceShipEngine*> engines);
    void addEnginesLinearNegativeX(std::vector<SpaceShipEngine*> engines);
    void addEnginesLinearPositiveY(std::vector<SpaceShipEngine*> engines);
    void addEnginesLinearNegativeY(std::vector<SpaceShipEngine*> engines);
    void addEnginesLinearPositiveZ(std::vector<SpaceShipEngine*> engines);
    void addEnginesLinearNegativeZ(std::vector<SpaceShipEngine*> engines);
    void addEnginesAngularPositiveX(std::vector<SpaceShipEngine*> engines);
    void addEnginesAngularNegativeX(std::vector<SpaceShipEngine*> engines);
    void addEnginesAngularPositiveY(std::vector<SpaceShipEngine*> engines);
    void addEnginesAngularNegativeY(std::vector<SpaceShipEngine*> engines);
    void addEnginesAngularPositiveZ(std::vector<SpaceShipEngine*> engines);
    void addEnginesAngularNegativeZ(std::vector<SpaceShipEngine*> engines);

    void updateEnginesThrust();

private:
    glm::dvec3 linearThrust;
    glm::dvec3 angularThrust;
    std::vector<SpaceShipEngine*> enginesLinearPositiveX;
    std::vector<SpaceShipEngine*> enginesLinearNegativeX;

    std::vector<SpaceShipEngine*> enginesLinearPositiveY;
    std::vector<SpaceShipEngine*> enginesLinearNegativeY;

    std::vector<SpaceShipEngine*> enginesLinearPositiveZ;
    std::vector<SpaceShipEngine*> enginesLinearNegativeZ;

    std::vector<SpaceShipEngine*> enginesAngularPositiveX;
    std::vector<SpaceShipEngine*> enginesAngularNegativeX;

    std::vector<SpaceShipEngine*> enginesAngularPositiveY;
    std::vector<SpaceShipEngine*> enginesAngularNegativeY;

    std::vector<SpaceShipEngine*> enginesAngularPositiveZ;
    std::vector<SpaceShipEngine*> enginesAngularNegativeZ;

    void setAllEnginesInVectorPower(std::vector<SpaceShipEngine*> &engines, double power);
};
