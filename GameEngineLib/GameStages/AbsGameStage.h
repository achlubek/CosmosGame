#pragma once

#include "SceneProvider.h"
#include "../Core/GameObjectTags.h"
class GameObject;
class CameraController;
class TimeProvider;
class GameContainer;

class AbsGameStage : public SceneProvider
{
public:
    AbsGameStage(GameContainer* container);
    virtual ~AbsGameStage();

    void addObject(GameObject* object);
    void removeObject(GameObject* object);
    void removeAllObjects();
    void updateObjects();
    std::vector<GameObject*> getAllGameObjects();
    std::vector<GameObject*> getGameObjectsByTag(GameObjectTags tag);
    GameObject* getGameObjectById(unsigned long id);

    void setTimeScale(double timeScale);
    double getTimeScale();

    TimeProvider* getTimeProvider();
    CameraController* getViewCamera();
    VEngine::UserInterface::UIRenderer* getUIRenderer();
    GameContainer* getGameContainer();

    virtual void onSwitchTo() = 0;
    virtual void onSwitchFrom() = 0;
    virtual void onDraw() = 0;

    virtual void onKeyDown(std::string key) = 0;
    virtual void onKeyUp(std::string key) = 0;
protected:
    virtual void onUpdate(double elapsed) = 0;
    virtual void onUpdateObject(GameObject* object, double elapsed) = 0;
    virtual void drawDrawableObjects(VEngine::Renderer::RenderStageInterface* stage, VEngine::Renderer::DescriptorSetInterface* set, double scale) override;

private:
    double timeScale = 1.0;
    GameContainer * container;
    TimeProvider* timeProvider;
    std::vector<GameObject*> activeObjects;
    VEngine::UserInterface::UIRenderer* ui;
    CameraController* viewCamera;
};

