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
    GameObject* player;
    double lastTime;
};

