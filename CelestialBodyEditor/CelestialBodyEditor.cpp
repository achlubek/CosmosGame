#include "stdafx.h"
#include "AbsGameContainer.h"
#include "AbsGameStage.h"
#include "GameContainer.h"
#include "FreeFlightGameStage.h"
#include "MainMenuGameStage.h"
#include "CinematicAnimationGameStage.h"
#include "CelestialBodyPreviewGameStage.h"
#include "ShipFactory.h"
#include "GameObject.h"
#include "ModelPreviewGameStage.h"
#include "AbsDrawableComponent.h"
#include "Model3dFactory.h"
#include "Transformation3DComponent.h"

int main(int argc, char** argv)
{
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");
    GameContainer* game = new GameContainer();

    // execname preview type filename
    if (argc == 4 && std::string(argv[1]) == "preview") {
        GameObject* object = nullptr;
        if (std::string(argv[2]) == "ship") {
            object = game->getShipFactory()->build(std::string(argv[3]));
        } else if (std::string(argv[2]) == "model") {
            object = new GameObject();
            auto model = game->getModel3dFactory()->build(std::string(argv[3]));
            object->addComponent(new AbsDrawableComponent(model, glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0)));
            object->addComponent(new Transformation3DComponent(0.01, glm::dvec3(0.0)));
        }
        ModelPreviewGameStage* previewStage = new ModelPreviewGameStage(game);
        previewStage->addPreviewObject(object);
        game->registerStage("modelspreview", previewStage);
        game->setCurrentStage("modelspreview");
        game->startGameLoops();
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

