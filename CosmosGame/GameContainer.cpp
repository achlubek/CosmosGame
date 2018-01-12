#include "stdafx.h"
#include "GameContainer.h"
#include "INIReader.h"
#include "VulkanToolkit.h"
#include "CosmosRenderer.h"
#include "AssetLoader.h"
#include "GameControls.h"
#include "PhysicalWorld.h"
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

    INIReader* configreader = new INIReader("settings.ini");
    vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"));

    cosmosRenderer = new CosmosRenderer(vulkanToolkit, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    assetLoader = new AssetLoader(vulkanToolkit);

    galaxyGenerator = cosmosRenderer->galaxy;

    int64_t galaxyedge = 12490000000;
    int64_t galaxythickness = 1524900000;
    for (int i = 0; i < 10000; i++) {
        galaxyGenerator->generateStar(galaxyedge, galaxythickness, 1.0, i);
        cosmosRenderer->nearbyStars.push_back(galaxyGenerator->generateStarInfo(i));
    }
    for (int i = 0; i < cosmosRenderer->nearbyStars[1234].planets.size(); i++) {
        cosmosRenderer->nearbyStars[1234].planets[i].orbitSpeed = 0.0;
    }


    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    gameControls = new GameControls(keyboard, mouse, "controls.ini");

    physicalWorld = new PhysicalWorld();

    database = new SQLiteDatabase(Media::getPath("gamedata.db"));

    shipFactory = new ShipFactory();

    // fuck it for now
    viewCamera = new CameraController();

    // a test
    auto testship = shipFactory->build(1);
    auto testspawnpos = cosmosRenderer->nearbyStars[111].planets[2].getPosition(0.0);
    auto testspawnradius = cosmosRenderer->nearbyStars[111].planets[2].radius;
    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(testspawnpos + glm::dvec3(0.0, 0.0, -testspawnradius * 2.0));
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

void GameContainer::drawDrawableObjects()
{
    auto observerPosition = viewCamera->getPosition();
    for (int i = 0; i < activeObjects.size(); i++) {
        auto comps = activeObjects[i]->getAllComponents();
        for (int g = 0; g < comps.size(); g++) {
            if (comps[g]->isDrawable()) {
                auto drawable = static_cast<AbsDrawableComponent*>(comps[g]);
                drawable->draw(observerPosition);
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
    cosmosRenderer->updateStars();
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->updateNearestStar(viewCamera->getPosition());
            cosmosRenderer->updateGravity(viewCamera->getPosition());

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
        cosmosRenderer->updatePlanetsAndMoon(viewCamera->getPosition());
        cosmosRenderer->draw();

        updateObjects();

        vulkanToolkit->poolEvents();
    }
    cosmosRenderer->unmapBuffers();
}
