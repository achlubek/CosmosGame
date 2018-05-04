#include "stdafx.h"
#include "GameStageCollection.h"


GameStageCollection::GameStageCollection()
{
}


GameStageCollection::~GameStageCollection()
{
}

void GameStageCollection::addStage(std::string name, AbsGameStage * stage)
{
    map[name] = stage;
}

AbsGameStage* GameStageCollection::getStage(std::string name)
{
    return map[name];
}
