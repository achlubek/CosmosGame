#pragma once

class GameObject;
class CameraController;
class TimeProvider;
class AbsGameContainer;
#include "SceneProvider.h"
class ParticlesRenderer;

class AbsGameStage : public SceneProvider
{
public:
    AbsGameStage(AbsGameContainer* container);
    ~AbsGameStage();

    void addObject(GameObject* object);
    void removeObject(GameObject* object);
    void removeAllObjects();
    void updateObjects();
    void setTimeScale(double timeScale);

    TimeProvider* getTimeProvider();
    CameraController* getViewCamera();
    UIRenderer* getUIRenderer();
    AbsGameContainer* getGameContainer();

    virtual void onSwitchTo() = 0;
    virtual void onSwitchFrom() = 0;
    virtual void onDraw() = 0;
protected:
    double getLastTime();
    virtual void onUpdate(double elapsed) = 0;
    virtual void onUpdateObject(GameObject* object, double elapsed) = 0;
    virtual void drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set, double scale) override;

private:
    double lastTime = 0.0;
    double timeScale = 1.0;
    AbsGameContainer * container;
    TimeProvider* timeProvider;
    std::vector<GameObject*> activeObjects;
    UIRenderer* ui;
    CameraController* viewCamera;
};

