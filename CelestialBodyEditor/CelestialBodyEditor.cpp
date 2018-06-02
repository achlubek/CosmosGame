#include "stdafx.h"
#include "AbsGameContainer.h"
#include "AbsGameStage.h"
#include "GameContainer.h"
#include "FreeFlightGameStage.h"
#include "MainMenuGameStage.h"
#include "CinematicAnimationGameStage.h"
#include "CelestialBodyPreviewGameStage.h"

int main(int argc, char** argv)
{

    if (argc == 0) {
        printf("Usage: editor.exe star [planet] [moon]\n");
        return 0;
    }

    int64_t targetStar = 334;
    int64_t targetPlanet = 3;
    int64_t targetMoon = 0;

    if (argc > 1) {
        targetStar = std::stol(argv[1]);
    }

    if (argc > 2) {
        targetPlanet = std::stol(argv[2]);
    }

    if (argc > 3) {
        targetMoon = std::stol(argv[3]);
    }

    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");


    GameContainer* game = new GameContainer();

    CelestialBodyPreviewGameStage* stage = new CelestialBodyPreviewGameStage(game, targetStar, targetPlanet, targetMoon);
    FreeFlightGameStage* stageflight = new FreeFlightGameStage(game);
    MainMenuGameStage* menustage = new MainMenuGameStage(game);
    CinematicAnimationGameStage* intro = new CinematicAnimationGameStage(game, "cinematic_scenario.txt", "mainmenu");

    game->registerStage("freeflight", stageflight);
    game->registerStage("preview", stage);
    game->registerStage("mainmenu", menustage);
    game->registerStage("intro-animation", intro);

    game->setCurrentStage("mainmenu");

    game->startGameLoops();


    return 0;
}

