#include "stdafx.h"
#include "OnClosestPlanetChangeEvent.h"


OnClosestPlanetChangeEvent::OnClosestPlanetChangeEvent(CelestialBody planet)
    : planet(planet)
{
}

OnClosestPlanetChangeEvent::~OnClosestPlanetChangeEvent()
{
}

std::string OnClosestPlanetChangeEvent::getName()
{
    return "OnClosestPlanetChangeEvent";
}
