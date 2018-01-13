#include "stdafx.h"
#include "GameContainer.h"

int main()
{ 
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    GameContainer * game = new GameContainer();
    game->startGameLoops();
    return 0;

}

