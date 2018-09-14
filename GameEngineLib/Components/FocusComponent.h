#pragma once
#include "AbsComponent.h"
class FocusComponent : public AbsComponent
{
public:
    FocusComponent();
    ~FocusComponent();

    virtual void update(double elapsed) override;
    virtual AbsComponent * clone() override;

    virtual std::string serialize() override;
    static FocusComponent* deserialize(std::string serializedString);
};

