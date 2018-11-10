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
    CosmosRenderer(VulkanToolkit* ivulkan, EventBus * eventBus, GalaxyContainer* galaxy, int iwidth, int iheight);
    ~CosmosRenderer();

    const double scale = 0.01;

    void recompileShaders(bool deleteOld); 

    void updateCameraBuffer(Camera* cam, double time);
    void draw(SceneProvider* scene, double time);

    VulkanImage* getOpaqueSurfaceDistanceImage();

    VulkanDescriptorSetLayout* getModelMRTLayout();

    VulkanImage* getUiOutputImage();

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

    VulkanToolkit* vulkan{ nullptr };

    CelestialBodyDataUpdater* celestialBodyDataUpdater{ nullptr };

    VulkanRenderStage* celestialStage{ nullptr };
    VulkanDescriptorSetLayout* rendererDataLayout{ nullptr };
    VulkanDescriptorSet* rendererDataSet{ nullptr };

    VulkanRenderStage* combineStage{ nullptr };
    VulkanDescriptorSetLayout* combineLayout{ nullptr };
    VulkanDescriptorSet* combineSet{ nullptr };

    VulkanDescriptorSetLayout* celestialBodyRenderSetLayout{ nullptr };

    VulkanDescriptorSetLayout* celestialBodyRaycastSharedSetLayout{ nullptr };
    VulkanDescriptorSet* celestialBodyRaycastSharedSet{ nullptr };
    VulkanDescriptorSetLayout* celestialBodyRaycastUniqueSetLayout{ nullptr };
    VulkanComputeStage* celestialBodyRaycastComputeStage;

    VulkanDescriptorSetLayout* celestialStarsBlitSetLayout{ nullptr };
    VulkanDescriptorSet* celestiaStarsBlitSet{ nullptr };
    VulkanComputeStage* celestialStarsBlitComputeStage;

    VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout{ nullptr };
    VulkanRenderStage* celestialBodySurfaceRenderStage;

    VulkanGenericBuffer* cameraDataBuffer;
    VulkanGenericBuffer* raycastRequestsDataBuffer;
    VulkanGenericBuffer* planetsDataBuffer;
    VulkanGenericBuffer* moonsDataBuffer;

    VulkanImage* celestialAlphaImage;
    VulkanImage* celestialAdditiveImage;

    VulkanImage* surfaceRenderedAlbedoRoughnessImage;
    VulkanImage* surfaceRenderedEmissionImage;
    VulkanImage* surfaceRenderedNormalMetalnessImage;
    VulkanImage* surfaceRenderedDistanceImage;

    VulkanImage* renderedDepthImage;
    
    Object3dInfo* cube3dInfo;

    /*models rendering*/

    VulkanDescriptorSetLayout* modelMRTLayout{ nullptr };
    VulkanRenderStage* modelsStage{ nullptr };
    VulkanDescriptorSetLayout* modelsDataLayout{ nullptr };
    VulkanDescriptorSet* modelsDataSet{ nullptr };
    VulkanGenericBuffer* modelsDataBuffer;

    VulkanImage* outputImage;
    VulkanImage* uiOutputImage;
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

