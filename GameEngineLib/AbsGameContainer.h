#pragma once

class VulkanToolkit;
class AssetLoader;
class GameControls;
class SQLiteDatabase;
class Model3dFactory;
class GameObject;
class CameraController;
class TimeProvider;
#include "SceneProvider.h"

class AbsGameContainer : public SceneProvider
{
public:
    AbsGameContainer();
    ~AbsGameContainer();
    void addObject(GameObject* object);
    void removeObject(GameObject* object);
    void removeAllObjects();
    void updateObjects();
    virtual void drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set) override;
    static AbsGameContainer* getInstance();
    VulkanToolkit* getVulkanToolkit();
    SQLiteDatabase* getDatabase();
    GameControls* getControls();
    TimeProvider* getTimeProvider();
    glm::vec2 getResolution();
    VulkanDescriptorSetLayout* getModelMRTLayout();
    virtual void startGameLoops() = 0;
    virtual void onUpdate(double elapsed) = 0;
    virtual void onUpdateObject(GameObject* object, double elapsed) = 0;
    void setGlobalDrawingScale(double scale);
protected:
    VulkanToolkit* vulkanToolkit;
    AssetLoader* assetLoader;
    UIRenderer* ui;
    GameControls* gameControls;
    SQLiteDatabase* database;
    Model3dFactory* model3dFactory;
    std::vector<GameObject*> activeObjects;
    CameraController* viewCamera;
    TimeProvider* timeProvider;
    double lastTime;
    static AbsGameContainer* instance;
    double globalDrawingScale = 1.0;
    VulkanDescriptorSetLayout* modelMRTLayout{ nullptr };
};

