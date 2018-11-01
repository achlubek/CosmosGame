#pragma once
#include "AbsStateMachine.h"
class ShipOrientationControllerState : public AbsStateMachine
{
public:
    ShipOrientationControllerState();
    ~ShipOrientationControllerState();

    virtual bool isStateChangeAllowed(std::string oldState, std::string newState) override;
    virtual void onStateChange(std::string oldState, std::string newState) override;

    virtual std::string serialize() override;
    virtual void deserialize(AbsGameStage * stage, std::string serializedString) override;
};

