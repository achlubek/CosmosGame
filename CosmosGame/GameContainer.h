#pragma once

class VulkanToolkit;
class AssetLoader;
class GalaxyGenerator;
class CosmosRenderer;
class GameControls; 
class SQLiteDatabase;
class Model3dFactory;
class ModuleFactory;
class ShipFactory;
class GameObject;
class CameraController;
class TimeProvider;
class UIText;
#include "SceneProvider.h"

class GameContainer : public SceneProvider
{
public:
    GameContainer();
    ~GameContainer();
    ShipFactory* getShipFactory();
    void addObject(GameObject* object);
    void removeObject(GameObject* object);
    void removeAllObjects();
    void updateObjects();
    virtual void drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set) override;
    static GameContainer* getInstance();
    CosmosRenderer* getCosmosRenderer();
    VulkanToolkit* getVulkanToolkit();
    SQLiteDatabase* getDatabase();
    GameControls* getControls();
    TimeProvider* getTimeProvider();
    glm::vec2 getResolution();
    void startGameLoops();
private:
    VulkanToolkit* vulkanToolkit;
    AssetLoader* assetLoader;
    GalaxyGenerator* galaxyGenerator;
    CosmosRenderer* cosmosRenderer;
    UIRenderer* ui;
    GameControls* gameControls; 
    SQLiteDatabase* database;
    Model3dFactory* model3dFactory;
    ModuleFactory* moduleFactory;
    ShipFactory* shipFactory;
    std::vector<GameObject*> activeObjects;
    CameraController* viewCamera;
    TimeProvider* timeProvider;
    double lastTime;
    static GameContainer* instance;
    UIText* fpsText;
    UIText* gravityFluxText;
    UIText* starNameText;
    UIText* planetNameText;
    UIText* moonNameText;
    UIText* altitudeText;
    UIText* velocityText;
};

