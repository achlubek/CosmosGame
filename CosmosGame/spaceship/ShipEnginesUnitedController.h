#pragma once
#include "SpaceShipEngine.h"
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

