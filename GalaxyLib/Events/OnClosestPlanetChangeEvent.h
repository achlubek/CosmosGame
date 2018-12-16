#pragma once
#include "../CelestialBody.h"
class OnClosestPlanetChangeEvent : public IEvent
{
public:
    OnClosestPlanetChangeEvent(CelestialBody planet);
    ~OnClosestPlanetChangeEvent();

    virtual std::string getName() override;

    CelestialBody planet;
};

