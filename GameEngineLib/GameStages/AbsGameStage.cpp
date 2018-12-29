#include "stdafx.h"
#include "AbsGameStage.h"

AbsGameStage::AbsGameStage(GameContainer* icontainer)
    : container(icontainer)
{
    activeObjects = {};

    timeProvider = new TimeProvider();

    ui = new UIRenderer(container->getToolkit(), container->getCosmosRenderer()->getUiOutputImage(), container->getResolution().x, container->getResolution().y);

    viewCamera = new CameraController();

}

AbsGameStage::~AbsGameStage()
{
    safedelete(viewCamera);
    safedelete(ui);
    safedelete(timeProvider);

    for (int i = 0; i < activeObjects.size(); i++) {
        safedelete(activeObjects[i]);
    }
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
    double lastTime = timeProvider->getLastTime();
    // TODO hmm
    for (int i = 0; i < activeObjects.size(); i++) {
        onUpdateObject(activeObjects[i], (nowtime - lastTime));
        activeObjects[i]->update((nowtime - lastTime));
    }
    onUpdate((nowtime - lastTime));
    viewCamera->update((nowtime - lastTime) / timeScale);
    lastTime = nowtime;
    timeProvider->synchronizeLastTime();
}

std::vector<GameObject*> AbsGameStage::getAllGameObjects()
{
    return activeObjects;
}

std::vector<GameObject*> AbsGameStage::getGameObjectsByTag(GameObjectTags tag)
{
    auto result = std::vector<GameObject*>();
    for (auto object : activeObjects) {
        if (object->hasTag(tag)) {
            result.push_back(object);
        }
    }
    return result;
}

GameObject * AbsGameStage::getGameObjectById(unsigned long id)
{
    for (auto object : activeObjects) {
        if (object->getID() == id) {
            return object;
        }
    }
    return nullptr;
}

void AbsGameStage::setTimeScale(double itimeScale)
{
    timeScale = itimeScale;
}

double AbsGameStage::getTimeScale()
{
    return timeScale;
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

GameContainer * AbsGameStage::getGameContainer()
{
    return container;
}

void AbsGameStage::drawDrawableObjects(RenderStageInterface* stage, DescriptorSetInterface* set, double scale)
{
    auto observerPosition = viewCamera->getCamera()->getPosition();
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
