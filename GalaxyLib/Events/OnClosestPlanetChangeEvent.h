#pragma once
#include "../CelestialBody.h"
class OnClosestPlanetChangeEvent : public AbsEvent
{
public:
    OnClosestPlanetChangeEvent(CelestialBody planet);
    ~OnClosestPlanetChangeEvent();

    virtual std::string getName() override;

    CelestialBody planet;
};

