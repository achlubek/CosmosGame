#include "stdafx.h"
#include "AbsGameContainer.h"

AbsGameContainer* AbsGameContainer::instance = nullptr;

AbsGameContainer::AbsGameContainer()
{
    logger = new DebugLogger();
    logger->setDesiredSeverityThreshold(LogSeverity::Normal);
    logger->log(LogSeverity::Normal, "AbsGameContainer initialization starts");

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

    interpolator = new Interpolator();

    outputImage = vulkanToolkit->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    uiOutputImage = vulkanToolkit->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsRenderer = new ModelsRenderer(vulkanToolkit, width, height);
    outputScreenRenderer = new OutputScreenRenderer(vulkanToolkit, width, height, outputImage, uiOutputImage);
    particlesRenderer = new ParticlesRenderer(getVulkanToolkit(),
        width, height, getModelsRenderer()->getDistanceImage());


    gameControls->onKeyDown.add([&](std::string key) {
        if (currentStage != nullptr) {
            currentStage->onKeyDown(key);
        }
    });

    gameControls->onKeyUp.add([&](std::string key) {
        if (currentStage != nullptr) {
            currentStage->onKeyUp(key);
        }
    });
}


AbsGameContainer::~AbsGameContainer()
{
}

VulkanToolkit * AbsGameContainer::getVulkanToolkit()
{
    return vulkanToolkit;
}

SQLiteDatabase * AbsGameContainer::getDatabase()
{
    return database;
}

Model3dFactory * AbsGameContainer::getModel3dFactory()
{
    return model3dFactory;
}

GameControls * AbsGameContainer::getControls()
{
    return gameControls;
}

glm::vec2 AbsGameContainer::getResolution()
{
    return glm::vec2((float)width, (float)height);
}

ModelsRenderer * AbsGameContainer::getModelsRenderer()
{
    return modelsRenderer;
}

Interpolator * AbsGameContainer::getInterpolator()
{
    return interpolator;
}

AbsGameStage * AbsGameContainer::getCurrentStage()
{
    return currentStage;
}

AbsGameContainer * AbsGameContainer::getInstance()
{
    return instance;
}

void AbsGameContainer::setCurrentStage(std::string stagename)
{
    auto stage = stageCollection->getStage(stagename);
    if (currentStage != nullptr) {
        currentStage->onSwitchFrom();
    }
    currentStage = stage;
    currentStage->onSwitchTo();
}

void AbsGameContainer::registerStage(std::string name, AbsGameStage * stage)
{
    stageCollection->addStage(name, stage);
}

void AbsGameContainer::startGameLoops()
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
        currentStage->getTimeProvider()->update(time - lastTimeX);
        currentStage->getViewCamera()->getCamera()->updateFrustumCone();
        getParticlesRenderer()->updateCameraBuffer(currentStage->getViewCamera()->getCamera());
        lastTimeFloored = floored;
        lastTimeX = time;

        modelsRenderer->updateCameraBuffer(currentStage->getViewCamera()->getCamera());
        modelsRenderer->draw(currentStage);
        currentStage->getUIRenderer()->draw();


        getParticlesRenderer()->draw();
        onDraw();

        currentStage->onDraw();

        outputScreenRenderer->draw();

        currentStage->updateObjects();

        interpolator->update(currentStage->getTimeProvider()->getTime());

        vulkanToolkit->poolEvents();
    }
}

double AbsGameContainer::getFramesPerSecond()
{
    return fps;
}

double AbsGameContainer::getFrameLength()
{
    if (fps == 0.0) return 0.0;
    return 1.0 / fps;
}

VulkanImage * AbsGameContainer::getOutputImage()
{
    return outputImage;
}

VulkanImage * AbsGameContainer::getUiOutputImage()
{
    return uiOutputImage;
}

ParticlesRenderer * AbsGameContainer::getParticlesRenderer()
{
    return particlesRenderer;
}

void AbsGameContainer::setShouldClose(bool close)
{
    shouldClose = true;
}

DebugLogger * AbsGameContainer::getLogger()
{
    return logger;
}
