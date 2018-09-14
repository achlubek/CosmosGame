#pragma once
class AbsStateMachine
{
public:
    AbsStateMachine();
    virtual ~AbsStateMachine();
    void registerState(std::string stateName);
    void setInitialState(std::string stateName);
    void changeState(std::string newState);
    std::string getCurrentState();
    virtual bool isStateChangeAllowed(std::string oldState, std::string newState) = 0;
    virtual std::string serialize() = 0;
    virtual void deserialize(AbsGameStage* stage, std::string serializedString) = 0;
protected:
    virtual void onStateChange(std::string oldState, std::string newState) = 0;
    std::string currentState;
private:
    std::vector<std::string> states;
    bool initialStateSet;
    bool isValidState(std::string stateName);
    void throwInitialStateAlreadySet();
    void throwInitialStateNotSet();
    void throwStateNotFound(std::string stateName);
    void throwStateAlreadyExists(std::string stateName);
    void throwNotAllowedStateChange(std::string oldState, std::string newState);
};

