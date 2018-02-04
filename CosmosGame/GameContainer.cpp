#include "stdafx.h"
#include "GameContainer.h"
#include "INIReader.h"
#include "VulkanToolkit.h"
#include "CosmosRenderer.h"
#include "AssetLoader.h"
#include "GameControls.h" 
#include "SQLiteDatabase.h"
#include "ShipFactory.h"
#include "GameObject.h"
#include "AbsComponent.h"
#include "AbsDrawableComponent.h"
#include "Transformation3DComponent.h"
#include "CameraController.h"

GameContainer* GameContainer::instance = nullptr;

// kids, this is how to not do single responsibility principle
GameContainer::GameContainer()
{
    instance = this;
    activeObjects = {};

    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(galaxydb);

    INIReader* configreader = new INIReader("settings.ini");
    vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"));

    ui = new UIRenderer(vulkanToolkit, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    cosmosRenderer = new CosmosRenderer(vulkanToolkit, this, galaxy, ui->outputImage, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    assetLoader = new AssetLoader(vulkanToolkit);

    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    gameControls = new GameControls(keyboard, mouse, "controls.ini");
     
    gameControls->onKeyDown.add([&](std::string key) {
        if (key == "recompile_shaders") getCosmosRenderer()->recompileShaders(true);
    });

    database = new SQLiteDatabase(Media::getPath("gamedata.db"));

    shipFactory = new ShipFactory();

    // fuck it for now
    viewCamera = new CameraController();

    // a test
    auto testship = shipFactory->build(1);
    auto testspawnpos = cosmosRenderer->galaxy->getAllStars()[666].getPosition(0);
    auto testspawnradius = cosmosRenderer->galaxy->getAllStars()[666].radius;
    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(testspawnpos + glm::dvec3(0.0, testspawnradius * 3.0, -testspawnradius * 3.0));
    activeObjects.push_back(testship);
    viewCamera->setTarget(activeObjects[0]);
}


GameContainer::~GameContainer()
{
}

ShipFactory * GameContainer::getShipFactory()
{
    return shipFactory;
}

void GameContainer::addObject(GameObject * object)
{
    activeObjects.push_back(object);
}

void GameContainer::removeObject(GameObject * object)
{
    auto found = std::find(activeObjects.begin(), activeObjects.end(), object);

    if (found != activeObjects.end()) {
        activeObjects.erase(found);
    }
}

void GameContainer::removeAllObjects()
{
    activeObjects.clear();
}

void GameContainer::updateObjects()
{
    double nowtime = glfwGetTime();
    for (int i = 0; i < activeObjects.size(); i++) {
        activeObjects[i]->update(nowtime - lastTime);
    }
    viewCamera->update(nowtime - lastTime);
    lastTime = nowtime;
}

void GameContainer::drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set)
{
    auto observerPosition = viewCamera->getPosition();
    for (int i = 0; i < activeObjects.size(); i++) {
        auto comps = activeObjects[i]->getAllComponents();
        for (int g = 0; g < comps.size(); g++) {
            if (comps[g]->isDrawable()) {
                auto drawable = static_cast<AbsDrawableComponent*>(comps[g]);
                drawable->draw(observerPosition, stage, set);
            }
        }
    }
}

GameContainer * GameContainer::getInstance()
{
    return instance;
}

CosmosRenderer * GameContainer::getCosmosRenderer()
{
    return cosmosRenderer;
}

VulkanToolkit * GameContainer::getVulkanToolkit()
{
    return vulkanToolkit;
}

SQLiteDatabase * GameContainer::getDatabase()
{
    return database;
}

GameControls * GameContainer::getControls()
{
    return gameControls;
}

glm::vec2 GameContainer::getResolution()
{
    return glm::vec2((float)vulkanToolkit->windowWidth, (float)vulkanToolkit->windowHeight);
}

void GameContainer::startGameLoops()
{
    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStarsBuffer();
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->galaxy->update(viewCamera->getPosition());

        }
    });
    background1.detach();
    int frames = 0;
    double lastTime = 0.0;
    double lastRawTime = 0.0;
    while (!vulkanToolkit->shouldCloseWindow()) {
        frames++;
        double time = glfwGetTime();
        double nowtime = floor(time);
        if (nowtime != lastTime) {
            printf("FPS %d\n", frames);
            frames = 0;
        }
        double elapsed_x100 = (float)(100.0 * (time - lastRawTime));
        double elapsed = (float)((time - lastRawTime));
        lastRawTime = time;
        lastTime = nowtime;

        //

        cosmosRenderer->updateCameraBuffer(viewCamera->getInternalCamera(), viewCamera->getPosition());
        cosmosRenderer->draw();

        updateObjects();

        vulkanToolkit->poolEvents();
    }
    cosmosRenderer->unmapBuffers();
}
