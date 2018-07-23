#pragma once

class VulkanToolkit;
class AssetLoader;
class GameControls;
class SQLiteDatabase;
class Model3dFactory;
class Interpolator;
class ModelsRenderer;
class AbsGameStage;
class OutputScreenRenderer;
class GameStageCollection;
class ParticlesRenderer;

class AbsGameContainer
{
public:
    AbsGameContainer();
    ~AbsGameContainer();

    static AbsGameContainer* getInstance();
    VulkanToolkit* getVulkanToolkit();
    SQLiteDatabase* getDatabase();
    Model3dFactory* getModel3dFactory();
    GameControls* getControls();
    glm::vec2 getResolution();
    ModelsRenderer* getModelsRenderer();
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
protected:
    virtual void onDrawingStart() = 0;
    virtual void onDraw() = 0;
private:
    VulkanToolkit* vulkanToolkit;
    GameControls* gameControls;
    SQLiteDatabase* database;
    Model3dFactory* model3dFactory;
    Interpolator* interpolator;
    ModelsRenderer* modelsRenderer;
    static AbsGameContainer* instance;
    AbsGameStage* currentStage{ nullptr };
    double fps{ 0 };
    VulkanImage* outputImage;
    VulkanImage* uiOutputImage;
    OutputScreenRenderer* outputScreenRenderer;
    GameStageCollection* stageCollection;
    ParticlesRenderer* particlesRenderer;
    bool shouldClose = false;
};

