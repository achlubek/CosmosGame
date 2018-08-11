#pragma once

class AssetLoader;
class GalaxyGenerator;
class CosmosRenderer;
class GameControls; 
class SQLiteDatabase;
class Model3dFactory;
class ParticleSystemFactory;
class ModuleFactory;
class ShipFactory;
class PlayerFactory;
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
    PlayerFactory* getPlayerFactory();
    CosmosRenderer* getCosmosRenderer();
    static GameContainer* getInstance();

private:
    virtual void onDraw() override;
    virtual void onDrawingStart() override;

    GalaxyGenerator* galaxyGenerator;
    CosmosRenderer* cosmosRenderer;
    ModuleFactory* moduleFactory;
    ParticleSystemFactory* particleSystemFactory;
    ShipFactory* shipFactory;
    PlayerFactory* playerFactory;
};

