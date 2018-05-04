#pragma once

class VulkanToolkit;
class AssetLoader;
class GameControls;
class SQLiteDatabase;
class Model3dFactory;
class Interpolator;
class ModelsRenderer;
class AbsGameStage;

class AbsGameContainer
{
public:
    AbsGameContainer();
    ~AbsGameContainer();

    static AbsGameContainer* getInstance();
    VulkanToolkit* getVulkanToolkit();
    SQLiteDatabase* getDatabase();
    AssetLoader* getAssetLoader();
    Model3dFactory* getModel3dFactory();
    GameControls* getControls();
    glm::vec2 getResolution();
    ModelsRenderer* getModelsRenderer();
    Interpolator* getInterpolator();
    AbsGameStage* getCurrentStage();
    void setCurrentStage(AbsGameStage* stage);
    void startGameLoops();
protected:
    virtual void onDrawingStart() = 0;
    virtual void onDraw() = 0;
private:
    VulkanToolkit* vulkanToolkit;
    AssetLoader* assetLoader;
    GameControls* gameControls;
    SQLiteDatabase* database;
    Model3dFactory* model3dFactory;
    Interpolator* interpolator;
    ModelsRenderer* modelsRenderer;
    static AbsGameContainer* instance;
    AbsGameStage* currentStage;
};

