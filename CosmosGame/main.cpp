#include "stdafx.h"
#include "GameContainer.h"
#include "FreeFlightGameStage.h"

int main()
{ 
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    GameContainer * game = new GameContainer();
    FreeFlightGameStage * stage = new FreeFlightGameStage(game);
    game->setCurrentStage(stage);
    game->startGameLoops();
    return 0;

}

