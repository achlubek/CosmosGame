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
#include "AbsGameContainer.h"

class GameContainer : public AbsGameContainer
{
public:
    GameContainer();
    ~GameContainer();
    ShipFactory* getShipFactory();
    CosmosRenderer* getCosmosRenderer();
    virtual void startGameLoops() override;
    virtual void onUpdate(double elapsed) override;
    virtual void onUpdateObject(GameObject* object, double elapsed) override;
    static GameContainer* getInstance();

private:
    GalaxyGenerator* galaxyGenerator;
    CosmosRenderer* cosmosRenderer;
    ModuleFactory* moduleFactory;
    ShipFactory* shipFactory;
    UIText* fpsText;
    UIText* gravityFluxText;
    UIText* starNameText;
    UIText* planetNameText;
    UIText* moonNameText;
    UIText* altitudeText;
    UIText* velocityText;
};

