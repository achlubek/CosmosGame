#include "stdafx.h"
#include "OnClosestMoonChangeEventHandler.h"


OnClosestMoonChangeEventHandler::OnClosestMoonChangeEventHandler(CosmosRenderer* cosmosRenderer)
    : cosmosRenderer(cosmosRenderer)
{
}

OnClosestMoonChangeEventHandler::~OnClosestMoonChangeEventHandler()
{
}

std::string OnClosestMoonChangeEventHandler::getSupportedName()
{
    return "OnClosestMoonChangeEvent";
}

void OnClosestMoonChangeEventHandler::handle(AbsEvent * event)
{
    auto castEvent = static_cast<OnClosestMoonChangeEvent*>(event);
    cosmosRenderer->onClosestMoonChange(castEvent->moon);
}
