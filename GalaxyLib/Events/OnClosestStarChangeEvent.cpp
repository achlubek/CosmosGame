#include "stdafx.h"
#include "OnClosestStarChangeEvent.h"


OnClosestStarChangeEvent::OnClosestStarChangeEvent(Star star)
    : star(star)
{
}

OnClosestStarChangeEvent::~OnClosestStarChangeEvent()
{
}

std::string OnClosestStarChangeEvent::getName()
{
    return "OnClosestStarChangeEvent";
}
