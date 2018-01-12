#pragma once

class VulkanToolkit;
class AssetLoader;
class GalaxyGenerator;
class CosmosRenderer;
class GameControls;
class PhysicalWorld;
class SQLiteDatabase;
class ShipFactory;
class GameObject;
class CameraController;

class GameContainer
{
public:
    GameContainer();
    ~GameContainer();
    ShipFactory* getShipFactory();
    void addObject(GameObject* object);
    void removeObject(GameObject* object);
    void removeAllObjects();
    void updateObjects();
    void drawDrawableObjects();
    static GameContainer* getInstance();
    CosmosRenderer* getCosmosRenderer();
    VulkanToolkit* getVulkanToolkit();
    SQLiteDatabase* getDatabase();
    GameControls* getControls();
    glm::vec2 getResolution();
    void startGameLoops();
private:
    VulkanToolkit* vulkanToolkit;
    AssetLoader* assetLoader;
    GalaxyGenerator* galaxyGenerator;
    CosmosRenderer* cosmosRenderer;
    GameControls* gameControls;
    PhysicalWorld* physicalWorld;
    SQLiteDatabase* database;
    ShipFactory* shipFactory;
    std::vector<GameObject*> activeObjects;
    CameraController* viewCamera;
    double lastTime;
    static GameContainer* instance;
};

