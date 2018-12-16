#pragma once

class GalaxyGenerator;
class CosmosRenderer;
class GameControls; 
class SQLiteDatabase;
class Model3dFactory;
class ModuleFactory;
class ShipFactory;
class PlayerFactory;
class GameObject;
class CameraController;
class TimeProvider;
class Serializer;
class GameControls;
class SQLiteDatabase;
class Model3dFactory;
class Interpolator;
class AbsGameStage;
class OutputScreenRenderer;
class GameStageCollection;
class DebugLogger;
class IGlobalService;

class GameContainer
{
public:
    GameContainer();
    ~GameContainer();

    static GameContainer* getInstance();

    ShipFactory* getShipFactory();
    PlayerFactory* getPlayerFactory();
    CosmosRenderer* getCosmosRenderer();
    Serializer* getSerializer();
    VulkanToolkit* getVulkanToolkit();
    SQLiteDatabase* getDatabase();
    Model3dFactory* getModel3dFactory();
    GameControls* getControls();
    glm::vec2 getResolution();
    Interpolator* getInterpolator();
    AbsGameStage* getCurrentStage();
    void setCurrentStage(std::string stage);
    void registerStage(std::string name, AbsGameStage* stage);
    void startGameLoops();
    double getFramesPerSecond();
    double getFrameLength();
    void setShouldClose(bool close);
    DebugLogger* getLogger();
    void setCurrentSunDirection(glm::mat4 mat);
private:
    void onDraw();
    void onDrawingStart();


    VulkanToolkit* vulkanToolkit;
    GameControls* gameControls;
    SQLiteDatabase* database;
    Model3dFactory* model3dFactory;
    Interpolator* interpolator;
    static GameContainer* instance;
    AbsGameStage* currentStage{ nullptr };
    double fps{ 0 };
    GameStageCollection* stageCollection;
    DebugLogger* logger;
    glm::mat4 currentSunDirection;
    bool shouldClose = false;
    int width, height;
    std::vector<IGlobalService*> globalServices;

    GalaxyGenerator* galaxyGenerator;
    CosmosRenderer* cosmosRenderer;
    ModuleFactory* moduleFactory;
    ShipFactory* shipFactory;
    PlayerFactory* playerFactory;
    Serializer * serializer;
};

