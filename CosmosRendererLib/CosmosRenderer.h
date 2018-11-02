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

class CosmosRenderer
{
public:
    CosmosRenderer(VulkanToolkit* ivulkan, GalaxyContainer* galaxy, int iwidth, int iheight);
    ~CosmosRenderer();

    const double scale = 0.01;

    void recompileShaders(bool deleteOld); 

    void updateCameraBuffer(Camera* cam, double time);
    void draw(SceneProvider* scene, double time);

    VulkanImage* getOpaqueSurfaceDistanceImage();

    VulkanDescriptorSetLayout* getModelMRTLayout();

    GalaxyContainer* getGalaxy();
    double getExposure();
    void setExposure(double value);

    void setRaycastPoints(std::vector<glm::dvec3> points);
    std::vector<glm::dvec3> getRaycastPoints();
    RenderedCelestialBody* getRenderableForCelestialBody(CelestialBody body);
private:
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

    VulkanRenderStage* celestialStage{ nullptr };
    VulkanDescriptorSetLayout* rendererDataLayout{ nullptr };
    VulkanDescriptorSet* rendererDataSet{ nullptr };

    VulkanRenderStage* combineStage{ nullptr };
    VulkanDescriptorSetLayout* combineLayout{ nullptr };
    VulkanDescriptorSet* combineSet{ nullptr };


    VulkanDescriptorSetLayout* celestialBodyDataSetLayout{ nullptr };
    VulkanDescriptorSetLayout* celestialBodyRenderSetLayout{ nullptr };
    VulkanComputeStage* celestialDataUpdateComputeStage;

    VulkanDescriptorSetLayout* celestialBodyRaycastSharedSetLayout{ nullptr };
    VulkanDescriptorSet* celestialBodyRaycastSharedSet{ nullptr };
    VulkanDescriptorSetLayout* celestialBodyRaycastUniqueSetLayout{ nullptr };
    VulkanComputeStage* celestialBodyRaycastComputeStage;

    VulkanDescriptorSetLayout* celestialStarsBlitSetLayout{ nullptr };
    VulkanDescriptorSet* celestiaStarsBlitSet{ nullptr };
    VulkanComputeStage* celestialStarsBlitComputeStage;


    VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout{ nullptr };
    VulkanRenderStage* celestialBodySurfaceRenderStage;

    VulkanDescriptorSetLayout* celestialBodyWaterSetLayout{ nullptr };
    VulkanRenderStage* celestialBodyWaterRenderStage;

    VulkanDescriptorSetLayout* celestialShadowMapSetLayout{ nullptr };
    VulkanDescriptorSetLayout* shadowMapDataSetLayout{ nullptr };
    std::vector<VulkanRenderStage*> celestialShadowMapRenderStages = {};
    std::vector<VulkanRenderStage*> modelsShadowMapRenderStages = {};

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
    VulkanImage* waterRenderedNormalMetalnessImage;
    VulkanImage* waterRenderedDistanceImage;

    std::vector<VulkanImage*> shadowmaps = {};
    std::vector<VulkanGenericBuffer*> shadowmapsBuffers = {};
    std::vector<VulkanDescriptorSet*> shadowmapsDataSets = {};
    VulkanImage* shadowmapsDepthMap;
    VulkanDescriptorSetLayout* shadowMapsCollectionLayout{ nullptr };
    VulkanDescriptorSet* shadowMapsCollectionSet{ nullptr };
    std::vector<double> shadowmapsDivisors = {2.0, 17.0, 75.0};
    const int shadowMapWidth = 2048;
    const int shadowMapHeight = 2048;
    
    Object3dInfo* cube3dInfo;

    /*models rendering*/

    VulkanDescriptorSetLayout* modelMRTLayout{ nullptr };
    VulkanRenderStage* modelsStage{ nullptr };
    VulkanRenderStage* modelsShadowStage{ nullptr };
    VulkanDescriptorSetLayout* modelsDataLayout{ nullptr };
    VulkanDescriptorSet* modelsDataSet{ nullptr };
    VulkanGenericBuffer* modelsDataBuffer;

    VulkanImage* outputImage;
    VulkanImage* uiOutputImage;
    OutputScreenRenderer* outputScreenRenderer;

    /*end models rendering*/

    double exposure = 0.0003;

    glm::dvec3 observerCameraPosition;
    double closestSurfaceDistance;
    
    void onClosestStarChange(Star star);
    void onClosestPlanetChange(CelestialBody planet);

    std::vector<RenderedCelestialBody*> renderablePlanets;
    std::vector<RenderedCelestialBody*> renderableMoons;

    CommandBus* internalCommandBus;

    volatile bool readyForDrawing = false;
    volatile bool firstRecordingDone = false;

    int cascadeCounter = 0;

//#define PERFORMANCE_DEBUG
    double measurementStopwatch = 0.0;
    void measureTimeStart();
    void measureTimeEnd(std::string name);

};

