#pragma once

class SceneProvider;
class TimeProvider;
class StarsRenderer;
class Camera;
class GalaxyContainer;
class SubdividedMeshesProvider;
class RenderedCelestialBody;
class CommandBus;
class OutputScreenRenderer;
class CelestialBodyDataUpdater;

class CosmosRenderer
{
public:
    CosmosRenderer(ToolkitInterface* toolkit, EventBus * eventBus, GalaxyContainer* galaxy, int iwidth, int iheight);
    ~CosmosRenderer();

    const double scale = 0.01;

    void recompileShaders(bool deleteOld); 

    void updateCameraBuffer(Camera* cam, double time);
    void draw(SceneProvider* scene, double time);

    ImageInterface* getOpaqueSurfaceDistanceImage();

    DescriptorSetLayoutInterface* getModelMRTLayout();

    ImageInterface* getUiOutputImage();

    GalaxyContainer* getGalaxy();
    double getExposure();
    void setExposure(double value);

    void setRaycastPoints(std::vector<glm::dvec3> points);
    std::vector<glm::dvec3> getRaycastPoints();
    RenderedCelestialBody* getRenderableForCelestialBody(CelestialBody body);

    void onClosestStarChange(Star star);
    void onClosestPlanetChange(CelestialBody planet);
    void onClosestMoonChange(CelestialBody moon);
private:
    EventBus * eventBus;

    GalaxyContainer* galaxy;

    StarsRenderer* starsRenderer;

    SubdividedMeshesProvider* subdividedMeshesProvider;

    std::vector<glm::dvec3> raycastPoints = {};

    //Camera* internalCamera; 

    int lastPlanetId;

    int shadowMapRoundRobinCounter = 0;

    int width{ 0 };
    int height{ 0 };

    ToolkitInterface* toolkit{ nullptr };

    CelestialBodyDataUpdater* celestialBodyDataUpdater{ nullptr };

    RenderStageInterface* celestialStage{ nullptr };
    DescriptorSetLayoutInterface* rendererDataLayout{ nullptr };
    DescriptorSetInterface* rendererDataSet{ nullptr };

    RenderStageInterface* combineStage{ nullptr };
    DescriptorSetLayoutInterface* combineLayout{ nullptr };
    DescriptorSetInterface* combineSet{ nullptr };

    DescriptorSetLayoutInterface* celestialBodyRenderSetLayout{ nullptr };

    DescriptorSetLayoutInterface* celestialBodyRaycastSharedSetLayout{ nullptr };
    DescriptorSetInterface* celestialBodyRaycastSharedSet{ nullptr };
    DescriptorSetLayoutInterface* celestialBodyRaycastUniqueSetLayout{ nullptr };
    ComputeStageInterface* celestialBodyRaycastComputeStage;

    DescriptorSetLayoutInterface* celestialStarsBlitSetLayout{ nullptr };
    DescriptorSetInterface* celestiaStarsBlitSet{ nullptr };
    ComputeStageInterface* celestialStarsBlitComputeStage;

    DescriptorSetLayoutInterface* celestialBodySurfaceSetLayout{ nullptr };
    RenderStageInterface* celestialBodySurfaceRenderStage;

    GenericBufferInterface* cameraDataBuffer;
    GenericBufferInterface* raycastRequestsDataBuffer;
    GenericBufferInterface* planetsDataBuffer;
    GenericBufferInterface* moonsDataBuffer;

    ImageInterface* celestialAlphaImage;
    ImageInterface* celestialAdditiveImage;

    ImageInterface* surfaceRenderedAlbedoRoughnessImage;
    ImageInterface* surfaceRenderedEmissionImage;
    ImageInterface* surfaceRenderedNormalMetalnessImage;
    ImageInterface* surfaceRenderedDistanceImage;

    ImageInterface* renderedDepthImage;
    
    Object3dInfoInterface* cube3dInfo;

    /*models rendering*/

    DescriptorSetLayoutInterface* modelMRTLayout{ nullptr };
    RenderStageInterface* modelsStage{ nullptr };
    DescriptorSetLayoutInterface* modelsDataLayout{ nullptr };
    DescriptorSetInterface* modelsDataSet{ nullptr };
    GenericBufferInterface* modelsDataBuffer;

    ImageInterface* outputImage;
    ImageInterface* uiOutputImage;
    OutputScreenRenderer* outputScreenRenderer;

    /*end models rendering*/

    double exposure = 0.0003;

    glm::dvec3 observerCameraPosition;
    
    std::vector<RenderedCelestialBody*> renderablePlanets;
    std::vector<RenderedCelestialBody*> renderableMoons;

    volatile bool readyForDrawing = false;
    volatile bool firstRecordingDone = false;
    
//#define PERFORMANCE_DEBUG
    double measurementStopwatch = 0.0;
    void measureTimeStart();
    void measureTimeEnd(std::string name);

};

