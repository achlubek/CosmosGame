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


// kids, this is how to not do single responsibility principle
GameContainer::GameContainer()
{
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

    gameControls = new GameControls(keyboard, "controls.ini");

    physicalWorld = new PhysicalWorld();

    database = new SQLiteDatabase(Media::getPath("gamedata.db"));

    shipFactory = new ShipFactory();
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
    auto found = std::find(activeObjects.begin, activeObjects.end, object);

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
    lastTime = nowtime;
}

void GameContainer::drawDrawableObjects()
{
    auto playerTransformation = player->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);
    auto observerPosition = playerTransformation->getPosition();
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
