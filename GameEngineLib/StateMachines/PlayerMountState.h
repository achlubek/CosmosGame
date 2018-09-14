#pragma once
#include "AbsStateMachine.h"
class PlayerMountState : public AbsStateMachine
{
public:
    PlayerMountState();
    ~PlayerMountState();

    virtual bool isStateChangeAllowed(std::string oldState, std::string newState) override;
    virtual void onStateChange(std::string oldState, std::string newState) override;
    void setVehicle(GameObject* vehicle);
    GameObject* getVehicle();

    virtual std::string serialize() override;
    virtual void deserialize(AbsGameStage * stage, std::string serializedString) override;
private:
    GameObject * targetVehicle{ nullptr };

};

