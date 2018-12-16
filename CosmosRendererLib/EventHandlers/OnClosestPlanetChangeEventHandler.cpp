#include "stdafx.h"
#include "OnClosestPlanetChangeEventHandler.h"


OnClosestPlanetChangeEventHandler::OnClosestPlanetChangeEventHandler(CosmosRenderer* cosmosRenderer)
    : cosmosRenderer(cosmosRenderer)
{
}

OnClosestPlanetChangeEventHandler::~OnClosestPlanetChangeEventHandler()
{
}

std::string OnClosestPlanetChangeEventHandler::getSupportedName()
{
    return "OnClosestPlanetChangeEvent";
}

void OnClosestPlanetChangeEventHandler::handle(IEvent * event)
{
    auto castEvent = static_cast<OnClosestPlanetChangeEvent*>(event);
    cosmosRenderer->onClosestPlanetChange(castEvent->planet);
}
