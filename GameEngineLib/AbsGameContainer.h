#pragma once

class VulkanToolkit;
class AssetLoader;
class GameControls;
class SQLiteDatabase;
class Model3dFactory;
class GameObject;
class CameraController;
class TimeProvider;
class ModelsRenderer;
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
    virtual void drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set, double scale) override;
    static AbsGameContainer* getInstance();
    VulkanToolkit* getVulkanToolkit();
    SQLiteDatabase* getDatabase();
    AssetLoader* getAssetLoader();
    Model3dFactory* getModel3dFactory();
    GameControls* getControls();
    TimeProvider* getTimeProvider();
    glm::vec2 getResolution();
    ModelsRenderer* getModelsRenderer();
    VulkanDescriptorSetLayout* getModelMRTLayout();
    virtual void startGameLoops() = 0;
protected:
    double getLastTime();
    virtual void onUpdate(double elapsed) = 0;
    virtual void onUpdateObject(GameObject* object, double elapsed) = 0;
private:
    VulkanToolkit* vulkanToolkit;
    AssetLoader* assetLoader;
    UIRenderer* ui;
    GameControls* gameControls;
    SQLiteDatabase* database;
    Model3dFactory* model3dFactory;
    std::vector<GameObject*> activeObjects;
    CameraController* viewCamera;
    TimeProvider* timeProvider;
    ModelsRenderer* modelsRenderer;
    double lastTime;
    static AbsGameContainer* instance;
};

