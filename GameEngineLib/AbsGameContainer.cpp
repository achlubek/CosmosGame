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
#include <ctype.h>

AbsGameContainer* AbsGameContainer::instance = nullptr;

AbsGameContainer::AbsGameContainer()
{
    instance = this;
    activeObjects = {};

    timeProvider = new TimeProvider();

    INIReader* configreader = new INIReader("settings.ini");
    vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"), false, "Galaxy Game");

    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    ui = new UIRenderer(vulkanToolkit, mouse, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    assetLoader = new AssetLoader(vulkanToolkit);

    gameControls = new GameControls(keyboard, mouse, "controls.ini");

    model3dFactory = new Model3dFactory();

    viewCamera = new CameraController();
     
    modelMRTLayout = new VulkanDescriptorSetLayout(vulkanToolkit);
    modelMRTLayout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    modelMRTLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->compile();
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

GameControls * AbsGameContainer::getControls()
{
    return gameControls;
}

TimeProvider * AbsGameContainer::getTimeProvider()
{
    return nullptr;
}

glm::vec2 AbsGameContainer::getResolution()
{
    return glm::vec2((float)vulkanToolkit->windowWidth, (float)vulkanToolkit->windowHeight);
}

VulkanDescriptorSetLayout* AbsGameContainer::getModelMRTLayout()
{
    return modelMRTLayout;
}

void AbsGameContainer::setGlobalDrawingScale(double scale)
{
    globalDrawingScale = scale;
}

void AbsGameContainer::drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set)
{
    auto observerPosition = viewCamera->getPosition();
    for (int i = 0; i < activeObjects.size(); i++) {
        auto comps = activeObjects[i]->getAllComponents();
        for (int g = 0; g < comps.size(); g++) {
            if (comps[g]->isDrawable()) {
                auto drawable = static_cast<AbsDrawableComponent*>(comps[g]);
                drawable->draw(observerPosition, stage, set, globalDrawingScale);
            }
        }
    }
}

AbsGameContainer * AbsGameContainer::getInstance()
{
    return instance;
}