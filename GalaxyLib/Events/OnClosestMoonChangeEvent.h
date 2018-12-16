#pragma once
#include "../CelestialBody.h"
class OnClosestMoonChangeEvent : public IEvent
{
public:
    OnClosestMoonChangeEvent(CelestialBody moon);
    ~OnClosestMoonChangeEvent();

    virtual std::string getName() override;

    CelestialBody moon;
};

