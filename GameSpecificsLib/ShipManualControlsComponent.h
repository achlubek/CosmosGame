#pragma once
#include "AbsComponent.h"
#include "ComponentTypes.h"
class ShipManualControlsComponent : public AbsComponent
{
public:
    ShipManualControlsComponent();
    ~ShipManualControlsComponent();

    virtual void update(double elapsed) override;
    virtual AbsComponent * clone() override;
};

