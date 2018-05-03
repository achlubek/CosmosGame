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
#include <ctype.h>

AbsGameContainer* AbsGameContainer::instance = nullptr;

AbsGameContainer::AbsGameContainer()
{
    instance = this;
    activeObjects = {};

    timeProvider = new TimeProvider();

    INIReader* configreader = new INIReader("settings.ini");
    vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"), configreader->geti("enable_validation_layers") > 0, "Galaxy Game");

    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    ui = new UIRenderer(vulkanToolkit, mouse, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    assetLoader = new AssetLoader(vulkanToolkit);

    gameControls = new GameControls(keyboard, mouse, "controls.ini");

    model3dFactory = new Model3dFactory();

    viewCamera = new CameraController();
     
    modelsRenderer = new ModelsRenderer(vulkanToolkit, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    database = new SQLiteDatabase("gamedata.db");

    interpolator = new Interpolator();
}


AbsGameContainer::~AbsGameContainer()
{
}


void AbsGameContainer::addObject(GameObject * object)
{
    activeObjects.push_back(object);
}

void AbsGameContainer::removeObject(GameObject * object)
{
    auto found = std::find(activeObjects.begin(), activeObjects.end(), object);

    if (found != activeObjects.end()) {
        activeObjects.erase(found);
    }
}

void AbsGameContainer::removeAllObjects()
{
    activeObjects.clear();
}

void AbsGameContainer::updateObjects()
{
    double timescale = 1.0;
    double nowtime = glfwGetTime();
    for (int i = 0; i < activeObjects.size(); i++) {
        onUpdateObject(activeObjects[i], (nowtime - lastTime) * timescale);
        activeObjects[i]->update((nowtime - lastTime) * timescale);
    }
    onUpdate((nowtime - lastTime) * timescale);
    viewCamera->update(nowtime - lastTime);
    timeProvider->update((nowtime - lastTime) * timescale);
    lastTime = nowtime;
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

TimeProvider * AbsGameContainer::getTimeProvider()
{
    return timeProvider;
}

CameraController * AbsGameContainer::getViewCamera()
{
    return viewCamera;
}

UIRenderer * AbsGameContainer::getUIRenderer()
{
    return ui;
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

void AbsGameContainer::drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set, double scale)
{
    auto observerPosition = viewCamera->getPosition();
    for (int i = 0; i < activeObjects.size(); i++) {
        auto comps = activeObjects[i]->getAllComponents();
        for (int g = 0; g < comps.size(); g++) {
            if (comps[g]->isDrawable()) {
                auto drawable = static_cast<AbsDrawableComponent*>(comps[g]);
                drawable->draw(observerPosition, stage, set, scale);
            }
        }
    }
}

AbsGameContainer * AbsGameContainer::getInstance()
{
    return instance;
}

void AbsGameContainer::startGameLoops()
{
    onDrawingStart();
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

        modelsRenderer->updateCameraBuffer(getViewCamera()->getInternalCamera(), getViewCamera()->getPosition());
        modelsRenderer->draw(this);
        ui->draw();
        onDraw();

        updateObjects();

        interpolator->update(timeProvider->getTime());

        vulkanToolkit->poolEvents();
    }
}

double AbsGameContainer::getLastTime()
{
    return lastTime;
}
