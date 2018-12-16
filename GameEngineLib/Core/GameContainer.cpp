#include "stdafx.h"
#include "GameContainer.h"

GameContainer* GameContainer::instance = nullptr;

GameContainer::GameContainer()
    : globalServices({})
{

    logger = new DebugLogger();
    logger->setDesiredSeverityThreshold(LogSeverity::Normal);
    logger->log(LogSeverity::Normal, "GameContainer initialization starts");

    instance = this;

    stageCollection = new GameStageCollection();

    auto temporaryMedia = new Media();
    temporaryMedia->scanDirectory("../../media");
    INIReader* configreader = new INIReader(temporaryMedia, "settings.ini");
    width = configreader->geti("window_width");
    height = configreader->geti("window_height");
    vulkanToolkit = new VulkanToolkit(width, height, configreader->geti("enable_validation_layers") > 0, "Galaxy Game");
    vulkanToolkit->getMedia()->scanDirectory("../../media");
    vulkanToolkit->getMedia()->scanDirectory("../../shaders");

    gameControls = new GameControls(vulkanToolkit->getKeyboard(), vulkanToolkit->getMouse(), vulkanToolkit->getMedia(), "controls.ini");

    model3dFactory = new Model3dFactory(vulkanToolkit->getMedia());
    
    database = new SQLiteDatabase("gamedata.db");

    gameControls->onKeyDown.add([&](std::string key) { // todo refactor into event bus
        if (currentStage != nullptr) {
            currentStage->onKeyDown(key);
        }
    });

    gameControls->onKeyUp.add([&](std::string key) {
        if (currentStage != nullptr) {
            currentStage->onKeyUp(key);
        }
    });
    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto eventBus = new EventBus();

    auto galaxy = new GalaxyContainer(eventBus);
    galaxy->loadFromDatabase(galaxydb);

    auto vulkanToolkit = getVulkanToolkit();

    cosmosRenderer = new CosmosRenderer(vulkanToolkit, eventBus, galaxy, getResolution().x, getResolution().y);
    cosmosRenderer->setExposure(0.0001);
    
    moduleFactory = new ModuleFactory(getModel3dFactory(), vulkanToolkit->getMedia());
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

Serializer * GameContainer::getSerializer()
{
    return serializer;
}

GameContainer * GameContainer::getInstance()
{
    return instance;
}

void GameContainer::onDrawingStart()
{
    /*std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->getGalaxy()->update(getCurrentStage()->getViewCamera()->getCamera()->getPosition(), getCurrentStage()->getTimeProvider()->getTime());
        }
    });
    background1.detach();*/
}

void GameContainer::onDraw()
{
    //temporary
    cosmosRenderer->getGalaxy()->update(getCurrentStage()->getViewCamera()->getCamera()->getPosition(), getCurrentStage()->getTimeProvider()->getTime());

    cosmosRenderer->updateCameraBuffer(getCurrentStage()->getViewCamera()->getCamera(), getCurrentStage()->getTimeProvider()->getTime());
    cosmosRenderer->draw(getCurrentStage(), getCurrentStage()->getTimeProvider()->getTime());
}


VulkanToolkit * GameContainer::getVulkanToolkit()
{
    return vulkanToolkit;
}

SQLiteDatabase * GameContainer::getDatabase()
{
    return database;
}

Model3dFactory * GameContainer::getModel3dFactory()
{
    return model3dFactory;
}

GameControls * GameContainer::getControls()
{
    return gameControls;
}

glm::vec2 GameContainer::getResolution()
{
    return glm::vec2((float)width, (float)height);
}

Interpolator * GameContainer::getInterpolator()
{
    return interpolator;
}

AbsGameStage * GameContainer::getCurrentStage()
{
    return currentStage;
}

void GameContainer::setCurrentStage(std::string stagename)
{
    auto stage = stageCollection->getStage(stagename);
    if (currentStage != nullptr) {
        currentStage->onSwitchFrom();
    }
    currentStage = stage;
    currentStage->onSwitchTo();
}

void GameContainer::registerStage(std::string name, AbsGameStage * stage)
{
    stageCollection->addStage(name, stage);
}

void GameContainer::startGameLoops()
{
    onDrawingStart();
    int frames = 0;
    double lastTimeX = 0.0;
    double lastTimeFloored = 0.0;
    while (!vulkanToolkit->shouldCloseWindow() && !shouldClose) {
        frames++;
        double time = glfwGetTime();
        double floored = floor(time);
        if (floored != lastTimeFloored) {
            fps = frames;
            printf("FPS %d\n", frames);
            frames = 0;
        }
        currentStage->getTimeProvider()->update((time - lastTimeX) * currentStage->getTimeScale());
        currentStage->getViewCamera()->getCamera()->updateFrustumCone();
        lastTimeFloored = floored;
        lastTimeX = time;

        currentStage->getUIRenderer()->draw();

        onDraw();

        currentStage->onDraw();

        currentStage->updateObjects();

        vulkanToolkit->poolEvents();
    }
}

double GameContainer::getFramesPerSecond()
{
    return fps;
}

double GameContainer::getFrameLength()
{
    if (fps == 0.0) return 0.0;
    return 1.0 / fps;
}

void GameContainer::setShouldClose(bool close)
{
    shouldClose = true;
}

DebugLogger * GameContainer::getLogger()
{
    return logger;
}

void GameContainer::setCurrentSunDirection(glm::mat4 mat)
{
    currentSunDirection = mat;
}