#include "stdafx.h"
#include "AbsGameContainer.h"
#include "INIReader.h"
#include "VulkanToolkit.h"
#include "AssetLoader.h"
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
#include "AbsGameStage.h"
#include <ctype.h>

AbsGameContainer* AbsGameContainer::instance = nullptr;

AbsGameContainer::AbsGameContainer()
{
    instance = this;

    INIReader* configreader = new INIReader("settings.ini");
    vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"), configreader->geti("enable_validation_layers") > 0, "Galaxy Game");

    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    assetLoader = new AssetLoader(vulkanToolkit);

    gameControls = new GameControls(keyboard, mouse, "controls.ini");

    model3dFactory = new Model3dFactory();

    modelsRenderer = new ModelsRenderer(vulkanToolkit, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    database = new SQLiteDatabase("gamedata.db");

    interpolator = new Interpolator();
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

AssetLoader * AbsGameContainer::getAssetLoader()
{
    return assetLoader;
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
    return glm::vec2((float)vulkanToolkit->windowWidth, (float)vulkanToolkit->windowHeight);
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

void AbsGameContainer::setCurrentStage(AbsGameStage * stage)
{
    currentStage = stage;
}

void AbsGameContainer::startGameLoops()
{
    auto stage = getCurrentStage();
    onDrawingStart();
    stage->onDrawingStart();
    int frames = 0;
    double lastTime = 0.0;
    while (!vulkanToolkit->shouldCloseWindow()) {
        frames++;
        double time = glfwGetTime();
        double nowtime = floor(time);
        if (nowtime != lastTime) {
            printf("FPS %d\n", frames);
            frames = 0;
        }
        lastTime = nowtime;

        modelsRenderer->updateCameraBuffer(currentStage->getViewCamera()->getInternalCamera(), currentStage->getViewCamera()->getPosition());
        modelsRenderer->draw(currentStage);
        currentStage->getUIRenderer()->draw();
        onDraw();
        stage->onDraw();

        currentStage->updateObjects();

        interpolator->update(currentStage->getTimeProvider()->getTime());

        vulkanToolkit->poolEvents();
    }
}