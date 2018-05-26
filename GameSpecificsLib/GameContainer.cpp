#include "stdafx.h"
#include "GameContainer.h"
#include "INIReader.h"
#include "VulkanToolkit.h"
#include "CosmosRenderer.h"
#include "AssetLoader.h"
#include "GameControls.h" 
#include "SQLiteDatabase.h"
#include "Model3dFactory.h"
#include "ModuleFactory.h"
#include "ShipFactory.h"
#include "GameObject.h"
#include "AbsComponent.h"
#include "AbsDrawableComponent.h"
#include "Transformation3DComponent.h"
#include "CameraController.h"
#include "GalaxyContainer.h"
#include "TimeProvider.h"
#include "UIRenderer.h"
#include "ModelsRenderer.h"
#include "AbsGameStage.h"
#include "ParticlesRenderer.h"
#include <ctype.h>

// kids, this is how to not do single responsibility principle
GameContainer::GameContainer()
    : AbsGameContainer()
{
    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(galaxydb);

    auto vulkanToolkit = getVulkanToolkit();
    cosmosRenderer = new CosmosRenderer(vulkanToolkit, galaxy, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);
    cosmosRenderer->setExposure(0.0001);
    getModelsRenderer()->setRenderingScale(cosmosRenderer->scale);
    getParticlesRenderer()->setRenderingScale(cosmosRenderer->scale);

    moduleFactory = new ModuleFactory(getModel3dFactory());
    shipFactory = new ShipFactory(getModel3dFactory(), moduleFactory);

}


GameContainer::~GameContainer()
{
}

ShipFactory * GameContainer::getShipFactory()
{
    return shipFactory;
}


CosmosRenderer * GameContainer::getCosmosRenderer()
{
    return cosmosRenderer;
}

GameContainer * GameContainer::getInstance()
{
    return static_cast<GameContainer*>(AbsGameContainer::getInstance());
}

void GameContainer::onDrawingStart()
{
    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStarsBuffer();
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->getGalaxy()->update(getCurrentStage()->getViewCamera()->getPosition());

        }
    });
    background1.detach();
}
void GameContainer::onDraw()
{
    cosmosRenderer->updateCameraBuffer(getCurrentStage()->getViewCamera()->getInternalCamera(), getCurrentStage()->getViewCamera()->getPosition(), getCurrentStage()->getTimeProvider()->getTime());
    cosmosRenderer->draw(getCurrentStage()->getTimeProvider()->getTime());
}
