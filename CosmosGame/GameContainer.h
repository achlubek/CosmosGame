#pragma once

class VulkanToolkit;
class AssetLoader;
class GalaxyGenerator;
class CosmosRenderer;
class GameControls;
class PhysicalWorld;
class SQLiteDatabase;
class SpaceShipDatabaseManager;

class GameContainer
{
public:
    GameContainer();
    ~GameContainer();
private:
    VulkanToolkit* vulkanToolkit;
    AssetLoader* assetLoader;
    GalaxyGenerator* galaxyGenerator;
    CosmosRenderer* cosmosRenderer;
    GameControls* gameControls;
    PhysicalWorld* physicalWorld;
    SQLiteDatabase* database;
    SpaceShipDatabaseManager* shipDatabaseManager;
};

