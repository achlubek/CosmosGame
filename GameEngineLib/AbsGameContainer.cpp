#include "stdafx.h"
#include "AbsGameContainer.h"
#include "INIReader.h"
#include "GameControls.h" 
#include "SQLiteDatabase.h"
#include "Model3dFactory.h"
#include "GameObject.h"
#include "AbsComponent.h"
#include "AbsDrawableComponent.h"
#include "Transformation3DComponent.h"
#include "CameraController.h"
#include "TimeProvider.h"
#include "UIRenderer.h"
#include "Interpolator.h"
#include "ModelsRenderer.h"
#include "OutputScreenRenderer.h"
#include "AbsGameStage.h"
#include "GameStageCollection.h"
#include "ParticlesRenderer.h"
#include "SQLiteDatabase.h"
#include <ctype.h>

AbsGameContainer* AbsGameContainer::instance = nullptr;

AbsGameContainer::AbsGameContainer()
{
    instance = this;

    stageCollection = new GameStageCollection();

    INIReader* configreader = new INIReader("settings.ini");
    width = configreader->geti("window_width");
    height = configreader->geti("window_height");
    vulkanToolkit = new VulkanToolkit(width, height, configreader->geti("enable_validation_layers") > 0, "Galaxy Game");
    
    gameControls = new GameControls(vulkanToolkit->getKeyboard(), vulkanToolkit->getMouse(), "controls.ini");

    model3dFactory = new Model3dFactory();


    database = new SQLiteDatabase("gamedata.db");

    interpolator = new Interpolator();

    outputImage = vulkanToolkit->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    uiOutputImage = vulkanToolkit->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsRenderer = new ModelsRenderer(vulkanToolkit, width, height);
    outputScreenRenderer = new OutputScreenRenderer(vulkanToolkit, width, height, outputImage, uiOutputImage);
    particlesRenderer = new ParticlesRenderer(getVulkanToolkit(),
        width, height, getModelsRenderer()->getDistanceImage());
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
        getParticlesRenderer()->updateCameraBuffer(currentStage->getViewCamera()->getInternalCamera(), currentStage->getViewCamera()->getPosition());
        lastTimeFloored = floored;
        lastTimeX = time;

        modelsRenderer->updateCameraBuffer(currentStage->getViewCamera()->getInternalCamera(), currentStage->getViewCamera()->getPosition());
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
