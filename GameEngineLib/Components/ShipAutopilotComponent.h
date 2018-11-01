#pragma once
#include "AbsComponent.h"

class ShipAutopilotComponent : public AbsComponent
{
public:
    ShipAutopilotComponent();
    ~ShipAutopilotComponent();

    virtual void update(double elapsed) override;
    virtual AbsComponent * clone() override;

    virtual std::string serialize() override;
    static ShipAutopilotComponent* deserialize(std::string serializedString);

private:
    ShipOrientationControllerState * shipOrientationControllerState;
    PercentageDerivativeController orientationPitchContoller;
    PercentageDerivativeController orientationYawContoller;
};

