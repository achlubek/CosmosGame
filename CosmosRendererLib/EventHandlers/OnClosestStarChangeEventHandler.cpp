#include "stdafx.h"
#include "OnClosestStarChangeEventHandler.h"


OnClosestStarChangeEventHandler::OnClosestStarChangeEventHandler(CosmosRenderer* cosmosRenderer)
    : cosmosRenderer(cosmosRenderer)
{
}

OnClosestStarChangeEventHandler::~OnClosestStarChangeEventHandler()
{
}

std::string OnClosestStarChangeEventHandler::getSupportedName()
{
    return "OnClosestStarChangeEvent";
}

void OnClosestStarChangeEventHandler::handle(AbsEvent * event)
{
    auto castEvent = static_cast<OnClosestStarChangeEvent*>(event);
    cosmosRenderer->onClosestStarChange(castEvent->star);
}
