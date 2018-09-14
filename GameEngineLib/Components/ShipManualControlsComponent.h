#pragma once
#include "AbsComponent.h"
class ShipManualControlsComponent : public AbsComponent
{
public:
    ShipManualControlsComponent();
    ~ShipManualControlsComponent();

    virtual void update(double elapsed) override;
    virtual AbsComponent * clone() override;    
    
    virtual std::string serialize() override;
    static ShipManualControlsComponent* deserialize(std::string serializedString);

};

