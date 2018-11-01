#include "stdafx.h"
#include "ShipOrientationControllerState.h"


ShipOrientationControllerState::ShipOrientationControllerState()
    : AbsStateMachine()
{
    registerState("disabled");
    registerState("stable");
    registerState("target");
    registerState("prograde");
    registerState("retrograde");
    registerState("progravity");
    registerState("retrogravity");
    setInitialState("disabled");
}


ShipOrientationControllerState::~ShipOrientationControllerState()
{
}

bool ShipOrientationControllerState::isStateChangeAllowed(std::string oldState, std::string newState)
{
    return true;
}

void ShipOrientationControllerState::onStateChange(std::string oldState, std::string newState)
{
}

std::string ShipOrientationControllerState::serialize()
{
    std::stringstream s;
    s << "currentState=" << currentState << "\n";
    return s.str();
}

void ShipOrientationControllerState::deserialize(AbsGameStage * stage, std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    currentState = reader.gets("currentState");
}