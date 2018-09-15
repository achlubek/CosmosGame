#include "stdafx.h"
#include "GameContainer.h"

GameContainer::GameContainer()
    : AbsGameContainer()
{
    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(galaxydb);

    auto vulkanToolkit = getVulkanToolkit();

    cosmosRenderer = new CosmosRenderer(vulkanToolkit, galaxy, getResolution().x, getResolution().y);
    cosmosRenderer->setExposure(0.0001);
    getModelsRenderer()->setRenderingScale(1.0);
    getParticlesRenderer()->setRenderingScale(1.0);

    particleSystemFactory = new ParticleSystemFactory(vulkanToolkit->getMedia());
    moduleFactory = new ModuleFactory(getModel3dFactory(), particleSystemFactory, vulkanToolkit->getMedia());
    shipFactory = new ShipFactory(getModel3dFactory(), moduleFactory, vulkanToolkit->getMedia());
    playerFactory = new PlayerFactory();
    serializer = new Serializer(this);
}


GameContainer::~GameContainer()
{
}

ShipFactory * GameContainer::getShipFactory()
{
    return shipFactory;
}

PlayerFactory * GameContainer::getPlayerFactory()
{
    return playerFactory;
}


CosmosRenderer * GameContainer::getCosmosRenderer()
{
    return cosmosRenderer;
}

ParticleSystemFactory * GameContainer::getParticleSystemFactory()
{
    return particleSystemFactory;
}

Serializer * GameContainer::getSerializer()
{
    return serializer;
}

GameContainer * GameContainer::getInstance()
{
    return static_cast<GameContainer*>(AbsGameContainer::getInstance());
}

void GameContainer::onDrawingStart()
{
    cosmosRenderer->mapBuffers();
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->getGalaxy()->update(getCurrentStage()->getViewCamera()->getCamera()->getPosition(), getCurrentStage()->getTimeProvider()->getTime());
        }
    });
    background1.detach();
}

void GameContainer::onDraw()
{
    cosmosRenderer->updateCameraBuffer(getCurrentStage()->getViewCamera()->getCamera(), getCurrentStage()->getTimeProvider()->getTime());
    cosmosRenderer->draw(getCurrentStage()->getTimeProvider()->getTime());
}
