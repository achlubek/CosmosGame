#pragma once
#include "AbsComponent.h"
#include "ComponentTypes.h"
class PlayerManualControlsComponent : public AbsComponent
{
public:
    PlayerManualControlsComponent();
    ~PlayerManualControlsComponent();

    virtual void update(double elapsed) override;
    virtual AbsComponent * clone() override;
};


