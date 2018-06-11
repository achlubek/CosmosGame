#pragma once

class VulkanRenderer;
class VulkanToolkit;
class SceneProvider;
class TimeProvider;
class StarsRenderer;
#include "GalaxyContainer.h" 
#include "InvokeQueue.h" 

class CosmosRenderer
{
public:
    CosmosRenderer(VulkanToolkit* ivulkan, GalaxyContainer* galaxy, int iwidth, int iheight);
    ~CosmosRenderer();

    const double scale = 0.01;

    void recompileShaders(bool deleteOld); 

    void mapBuffers();
    void unmapBuffers();

    void updateCameraBuffer(Camera* cam, glm::dvec3 observerPosition, double time);
    void draw(double time);

    GalaxyContainer* getGalaxy();
    double getExposure();
    void setExposure(double value);
    void invokeOnDrawingThread(std::function<void(void)> func);
private:
    GalaxyContainer* galaxy;
    AssetLoader* assets;

    StarsRenderer* starsRenderer;

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

    VulkanDescriptorSetLayout* celestiaStarsBlitSetLayout{ nullptr };
    VulkanDescriptorSet* celestiaStarsBlitSet{ nullptr };
    VulkanComputeStage* celestialStarsBlitComputeStage;


    VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout{ nullptr };
    VulkanRenderStage* celestialBodySurfaceRenderStage;

    VulkanDescriptorSetLayout* celestialBodyWaterSetLayout{ nullptr };
    VulkanRenderStage* celestialBodyWaterRenderStage;

    VulkanDescriptorSetLayout* celestialShadowMapSetLayout{ nullptr };
    VulkanDescriptorSetLayout* shadowMapDataSetLayout{ nullptr };
    std::vector<VulkanRenderStage*> celestialShadowMapRenderStages = {};

    VulkanGenericBuffer* cameraDataBuffer;
    VulkanGenericBuffer* planetsDataBuffer;
    VulkanGenericBuffer* moonsDataBuffer;

    VulkanImage* celestialAlphaImage;
    VulkanImage* celestialAdditiveImage;

    VulkanImage* surfaceRenderedAlbedoRoughnessImage;
    VulkanImage* surfaceRenderedNormalMetalnessImage;
    VulkanImage* surfaceRenderedDistanceImage;
    VulkanImage* surfaceRenderedDepthImage;
    VulkanImage* waterRenderedNormalMetalnessImage;
    VulkanImage* waterRenderedDistanceImage;
    VulkanImage* waterRenderedDepthImage;

    std::vector<VulkanImage*> shadowmaps = {};
    std::vector<VulkanGenericBuffer*> shadowmapsBuffers = {};
    std::vector<VulkanDescriptorSet*> shadowmapsDataSets = {};
    VulkanImage* shadowmapsDepthMap;
    VulkanDescriptorSetLayout* shadowMapsCollectionLayout{ nullptr };
    VulkanDescriptorSet* shadowMapsCollectionSet{ nullptr };
    std::vector<double> shadowmapsDivisors = {1.0, 5.0, 25.0};
    const int shadowMapWidth = 1024;
    const int shadowMapHeight = 1024;
    
    Object3dInfo* cube3dInfo;

    Object3dInfo* icosphereLow;
    Object3dInfo* icosphereMedium;
    Object3dInfo* icosphereHigh;


    double exposure = 0.0003;

    glm::dvec3 observerCameraPosition;
    double closestSurfaceDistance;

    void* planetsDataBufferPointer;
    void* moonsDataBufferPointer;


    void onClosestStarChange(GeneratedStarInfo star);
    void onClosestPlanetChange(CelestialBody planet);

    std::vector<RenderedCelestialBody*> renderablePlanets;
    std::vector<RenderedCelestialBody*> renderableMoons;

    InvokeQueue updatingSafetyQueue;

    volatile bool readyForDrawing = false;
    volatile bool firstRecordingDone = false;

//#define PERFORMANCE_DEBUG
    double measurementStopwatch = 0.0;
    void measureTimeStart();
    void measureTimeEnd(std::string name);

    std::vector<std::tuple<glm::vec3, Object3dInfo*>> patchesLowPoly = {};
    std::vector<std::tuple<glm::vec3, Object3dInfo*>> patchesMediumPoly = {};
    std::vector<std::tuple<glm::vec3, Object3dInfo*>> patchesHighPoly = {};

    Object3dInfo* subdivide(Object3dInfo* info);
    std::vector<Object3dInfo*> splitTriangles(Object3dInfo* info);
};

