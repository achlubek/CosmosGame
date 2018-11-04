#include "stdafx.h"
#include "OnClosestMoonChangeEvent.h"


OnClosestMoonChangeEvent::OnClosestMoonChangeEvent(CelestialBody moon)
    : moon(moon)
{
}

OnClosestMoonChangeEvent::~OnClosestMoonChangeEvent()
{
}

std::string OnClosestMoonChangeEvent::getName()
{
    return "OnClosestMoonChangeEvent";
}
