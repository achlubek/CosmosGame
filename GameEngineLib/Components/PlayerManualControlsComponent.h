#pragma once
#include "AbsComponent.h"
class PlayerManualControlsComponent : public AbsComponent
{
public:
    PlayerManualControlsComponent();
    ~PlayerManualControlsComponent();

    virtual void update(double elapsed) override;
    virtual AbsComponent * clone() override;

    virtual std::string serialize() override;
    static PlayerManualControlsComponent* deserialize(std::string serializedString);
};


