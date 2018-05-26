#include "stdafx.h"
#include "AbsGameStage.h"
#include "stdafx.h"
#include "AbsGameContainer.h"
#include "INIReader.h"
#include "VulkanToolkit.h"
#include "AssetLoader.h"
#include "GameControls.h" 
#include "SQLiteDatabase.h"
#include "AbsGameContainer.h"
#include "GameObject.h"
#include "AbsComponent.h"
#include "AbsDrawableComponent.h"
#include "Transformation3DComponent.h"
#include "CameraController.h"
#include "TimeProvider.h"
#include "UIRenderer.h"
#include "ParticlesRenderer.h"
#include "ModelsRenderer.h"

AbsGameStage::AbsGameStage(AbsGameContainer* icontainer)
    : container(icontainer)
{
    activeObjects = {};

    timeProvider = new TimeProvider();

    ui = new UIRenderer(container->getVulkanToolkit(), container->getControls()->getRawMouse(), container->getUiOutputImage(), container->getVulkanToolkit()->windowWidth, container->getVulkanToolkit()->windowHeight);

    viewCamera = new CameraController();

}

AbsGameStage::~AbsGameStage()
{
}

void AbsGameStage::addObject(GameObject * object)
{
    activeObjects.push_back(object);
}

void AbsGameStage::removeObject(GameObject * object)
{
    auto found = std::find(activeObjects.begin(), activeObjects.end(), object);

    if (found != activeObjects.end()) {
        activeObjects.erase(found);
    }
}

void AbsGameStage::removeAllObjects()
{
    activeObjects.clear();
}

void AbsGameStage::updateObjects()
{
    double nowtime = timeProvider->getTime();
    double timescale = 1.0;
    for (int i = 0; i < activeObjects.size(); i++) {
        onUpdateObject(activeObjects[i], (nowtime - lastTime) * timescale);
        activeObjects[i]->update((nowtime - lastTime) * timescale);
    }
    onUpdate((nowtime - lastTime) * timescale);
    viewCamera->update(nowtime - lastTime);
    lastTime = nowtime;
}

TimeProvider * AbsGameStage::getTimeProvider()
{
    return timeProvider;
}

CameraController * AbsGameStage::getViewCamera()
{
    return viewCamera;
}

UIRenderer * AbsGameStage::getUIRenderer()
{
    return ui;
}

AbsGameContainer * AbsGameStage::getGameContainer()
{
    return container;
}

void AbsGameStage::drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set, double scale)
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

double AbsGameStage::getLastTime()
{
    return lastTime;
}
