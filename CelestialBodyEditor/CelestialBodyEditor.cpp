#include "stdafx.h"
#include "UtilitiesLib.h"
#include "CosmosRendererLib.h"
#include "SQLiteDatabase.h"
//#include "GameEngineLib.h"

class EmptySceneProvider : public SceneProvider
{
    virtual void drawDrawableObjects(VEngine::Renderer::VulkanRenderStage * stage, VEngine::Renderer::VulkanDescriptorSet * set, double scale) override
    {
    }
};

int main(int argc, char** argv)
{
    // very much a test now..

    int width = 1920;
    int height = 1080;
    auto vulkanToolkit = new VulkanToolkit(width, height, true, "Test");
    vulkanToolkit->getMedia()->scanDirectory("../../media");
    vulkanToolkit->getMedia()->scanDirectory("../../shaders");

    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(galaxydb);

    auto cosmosRenderer = new CosmosRenderer(vulkanToolkit, galaxy, width, height);
    cosmosRenderer->setExposure(14.1);

    Camera* camera = new Camera();
    camera->createProjectionPerspective(90.0, 16.0 / 9.0, 0.001f, 60000.0f);
    EmptySceneProvider* scene = new EmptySceneProvider();
 
    
    int frames = 0;
    double lastTimeX = 0.0;
    double lastTimeFloored = 0.0;
    while (!vulkanToolkit->shouldCloseWindow()) {
        frames++;
        double time = glfwGetTime();
        double floored = floor(time);
        if (floored != lastTimeFloored) {
            printf("FPS %d\n", frames);
            frames = 0;
        }
        lastTimeFloored = floored;
        camera->updateFrustumCone();
        cosmosRenderer->updateCameraBuffer(camera, time);
        cosmosRenderer->draw(scene, time);

        camera->setOrientation(camera->getOrientation() * glm::angleAxis(0.0001f, glm::vec3(1.0, 1.1, 0.2)));

        vulkanToolkit->poolEvents();
    }

    /*
    GameContainer* game = new GameContainer();

    // execname preview type filename
    if (argc == 4 && std::string(argv[1]) == "preview") {
        GameObject* object = nullptr;
        if (std::string(argv[2]) == "ship") {
            object = game->getShipFactory()->build(std::string(argv[3]));
        } else if (std::string(argv[2]) == "model") {
            object = new GameObject();
            auto model = game->getModel3dFactory()->build(std::string(argv[3]));
            object->addComponent(new AbsDrawableComponent(model, std::string(argv[3]), glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0)));
            object->addComponent(new Transformation3DComponent(0.01, glm::dvec3(0.0)));
        }
        ModelPreviewGameStage* previewStage = new ModelPreviewGameStage(game);
        previewStage->addPreviewObject(object);
        game->registerStage("modelspreview", previewStage);
        game->setCurrentStage("modelspreview");
        game->startGameLoops();
        return 0;
    }

    int64_t targetStar = 11;
    int64_t targetPlanet = 1;
    int64_t targetMoon = 0;
    bool isPreview = false;
    if (argc > 1) {
        isPreview = true;
        targetStar = std::stol(argv[1]);
    }

    if (argc > 2) {
        targetPlanet = std::stol(argv[2]);
    }

    if (argc > 3) {
        targetMoon = std::stol(argv[3]);
    }


    CelestialBodyPreviewGameStage* stage = new CelestialBodyPreviewGameStage(game, targetStar, targetPlanet, targetMoon);

    //FreeFlightGameStage* stageflight = serializer->deserializeFreeFlightGameStage(serializer->prepareNewDatabase("test.db"));
    FreeFlightGameStage* stageflight = new FreeFlightGameStage(game);
    stageflight->initializeNew();
   // MainMenuGameStage* menustage = new MainMenuGameStage(game);
   // CinematicAnimationGameStage* intro = new CinematicAnimationGameStage(game, "cinematic_scenario.txt", "mainmenu");

   // game->registerStage("freeflight", stageflight);
    game->registerStage("preview", stage);
  //  game->registerStage("mainmenu", menustage);
  //  game->registerStage("intro-animation", intro);

   // game->setCurrentStage("preview");
    game->setCurrentStage("preview");

    game->startGameLoops();
    */

    return 0;
}

