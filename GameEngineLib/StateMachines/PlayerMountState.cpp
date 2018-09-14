#include "stdafx.h"
#include "PlayerMountState.h"


PlayerMountState::PlayerMountState()
    : AbsStateMachine()
{
    registerState("not-mounted");
    registerState("mounted");
    setInitialState("not-mounted");
}


PlayerMountState::~PlayerMountState()
{
}

bool PlayerMountState::isStateChangeAllowed(std::string oldState, std::string newState)
{
    return true;
}

void PlayerMountState::onStateChange(std::string oldState, std::string newState)
{
}

GameObject * PlayerMountState::getVehicle()
{
    return targetVehicle;
}

std::string PlayerMountState::serialize()
{
    std::stringstream s;
    s << "targetVehicleId=" << (targetVehicle == nullptr ? 0 : targetVehicle->getID()) << "\n";
    s << "currentState=" << currentState << "\n";
    return s.str();
}

void PlayerMountState::deserialize(AbsGameStage * stage, std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    currentState = reader.gets("currentState");
    targetVehicle = stage->getGameObjectById(reader.geti64("targetVehicleId"));
}

void PlayerMountState::setVehicle(GameObject * vehicle)
{
    targetVehicle = vehicle;
}
