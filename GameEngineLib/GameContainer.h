#pragma once

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
class Serializer;
class GameControls;
class SQLiteDatabase;
class Model3dFactory;
class Interpolator;
class AbsGameStage;
class OutputScreenRenderer;
class GameStageCollection;
class ParticlesRenderer;
class DebugLogger;

class GameContainer
{
public:
    GameContainer();
    ~GameContainer();

    static GameContainer* getInstance();

    ShipFactory* getShipFactory();
    PlayerFactory* getPlayerFactory();
    CosmosRenderer* getCosmosRenderer();
    ParticleSystemFactory* getParticleSystemFactory();
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
    VulkanImage* getOutputImage();
    VulkanImage* getUiOutputImage();
    ParticlesRenderer* getParticlesRenderer();
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
    VulkanImage* outputImage;
    VulkanImage* uiOutputImage;
    OutputScreenRenderer* outputScreenRenderer;
    GameStageCollection* stageCollection;
    ParticlesRenderer* particlesRenderer;
    DebugLogger* logger;
    glm::mat4 currentSunDirection;
    bool shouldClose = false;
    int width, height;

    GalaxyGenerator* galaxyGenerator;
    CosmosRenderer* cosmosRenderer;
    ModuleFactory* moduleFactory;
    ParticleSystemFactory* particleSystemFactory;
    ShipFactory* shipFactory;
    PlayerFactory* playerFactory;
    Serializer * serializer;
};

