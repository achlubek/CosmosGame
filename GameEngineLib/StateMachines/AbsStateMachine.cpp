#include "stdafx.h"
#include "AbsStateMachine.h"


AbsStateMachine::AbsStateMachine()
    : states({}), currentState(""), initialStateSet(false)
{
}


AbsStateMachine::~AbsStateMachine()
{
}

void AbsStateMachine::registerState(std::string stateName)
{
    if (isValidState(stateName)) {
        throwStateAlreadyExists(stateName);
    }
    states.push_back(stateName);
}

void AbsStateMachine::setInitialState(std::string stateName)
{
    if (initialStateSet) {
        throwInitialStateAlreadySet();
    }
    if (!isValidState(stateName)) {
        throwStateNotFound(stateName);
    }
    currentState = stateName;
    initialStateSet = true;
}

void AbsStateMachine::changeState(std::string newState)
{
    if (!initialStateSet) {
        throwInitialStateNotSet();
    }
    if (!isValidState(newState)) {
        throwStateNotFound(newState);
    }
    if (!isStateChangeAllowed(currentState, newState)) {
        throwNotAllowedStateChange(currentState, newState);
    }
    onStateChange(currentState, newState);
    currentState = newState;
}

std::string AbsStateMachine::getCurrentState()
{
    return currentState;
}

bool AbsStateMachine::isValidState(std::string stateName)
{
    return std::find(states.begin(), states.end(), stateName) != states.end();
}

void AbsStateMachine::throwInitialStateAlreadySet()
{
    throw "Initial state already set";
}

void AbsStateMachine::throwInitialStateNotSet()
{
    throw "Initial state not set";
}

void AbsStateMachine::throwStateNotFound(std::string stateName)
{
    throw "State not found: '" + stateName + ";";
}

void AbsStateMachine::throwStateAlreadyExists(std::string stateName)
{
    throw "State already exists: '" + stateName + ";";
}

void AbsStateMachine::throwNotAllowedStateChange(std::string oldState, std::string newState)
{
    throw "State change not allowed, old state: '" + oldState + "', new state: '" + newState + "'";
}
