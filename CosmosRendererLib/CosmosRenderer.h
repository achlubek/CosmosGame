#pragma once

class SceneProvider;
class TimeProvider;
class StarsRenderer;
#include "GalaxyContainer.h" 
#include "InvokeQueue.h" 
#include "Camera.h" 

class CosmosRenderer
{
public:
    CosmosRenderer(VEngine::Renderer::VulkanToolkit* ivulkan, GalaxyContainer* galaxy, int iwidth, int iheight);
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

    StarsRenderer* starsRenderer;

    //Camera* internalCamera; 

    int lastPlanetId;

    int shadowMapRoundRobinCounter = 0;

    int width{ 0 };
    int height{ 0 };

    VEngine::Renderer::VulkanToolkit* vulkan{ nullptr };

    VEngine::Renderer::VulkanRenderStage* celestialStage{ nullptr };
    VEngine::Renderer::VulkanDescriptorSetLayout* rendererDataLayout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSet* rendererDataSet{ nullptr };

    VEngine::Renderer::VulkanRenderStage* combineStage{ nullptr };
    VEngine::Renderer::VulkanDescriptorSetLayout* combineLayout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSet* combineSet{ nullptr };


    VEngine::Renderer::VulkanDescriptorSetLayout* celestialBodyDataSetLayout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSetLayout* celestialBodyRenderSetLayout{ nullptr };
    VEngine::Renderer::VulkanComputeStage* celestialDataUpdateComputeStage;

    VEngine::Renderer::VulkanDescriptorSetLayout* celestiaStarsBlitSetLayout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSet* celestiaStarsBlitSet{ nullptr };
    VEngine::Renderer::VulkanComputeStage* celestialStarsBlitComputeStage;


    VEngine::Renderer::VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout{ nullptr };
    VEngine::Renderer::VulkanRenderStage* celestialBodySurfaceRenderStage;

    VEngine::Renderer::VulkanDescriptorSetLayout* celestialBodyWaterSetLayout{ nullptr };
    VEngine::Renderer::VulkanRenderStage* celestialBodyWaterRenderStage;

    VEngine::Renderer::VulkanDescriptorSetLayout* celestialShadowMapSetLayout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSetLayout* shadowMapDataSetLayout{ nullptr };
    std::vector<VEngine::Renderer::VulkanRenderStage*> celestialShadowMapRenderStages = {};

    VEngine::Renderer::VulkanGenericBuffer* cameraDataBuffer;
    VEngine::Renderer::VulkanGenericBuffer* planetsDataBuffer;
    VEngine::Renderer::VulkanGenericBuffer* moonsDataBuffer;

    VEngine::Renderer::VulkanImage* celestialAlphaImage;
    VEngine::Renderer::VulkanImage* celestialAdditiveImage;

    VEngine::Renderer::VulkanImage* surfaceRenderedAlbedoRoughnessImage;
    VEngine::Renderer::VulkanImage* surfaceRenderedNormalMetalnessImage;
    VEngine::Renderer::VulkanImage* surfaceRenderedDistanceImage;
    VEngine::Renderer::VulkanImage* surfaceRenderedDepthImage;
    VEngine::Renderer::VulkanImage* waterRenderedNormalMetalnessImage;
    VEngine::Renderer::VulkanImage* waterRenderedDistanceImage;
    VEngine::Renderer::VulkanImage* waterRenderedDepthImage;

    std::vector<VEngine::Renderer::VulkanImage*> shadowmaps = {};
    std::vector<VEngine::Renderer::VulkanGenericBuffer*> shadowmapsBuffers = {};
    std::vector<VEngine::Renderer::VulkanDescriptorSet*> shadowmapsDataSets = {};
    VEngine::Renderer::VulkanImage* shadowmapsDepthMap;
    VEngine::Renderer::VulkanDescriptorSetLayout* shadowMapsCollectionLayout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSet* shadowMapsCollectionSet{ nullptr };
    std::vector<double> shadowmapsDivisors = {1.0, 5.0, 25.0};
    const int shadowMapWidth = 1024;
    const int shadowMapHeight = 1024;
    
    VEngine::Renderer::Object3dInfo* cube3dInfo;

    VEngine::Renderer::Object3dInfo* icosphereLow;
    VEngine::Renderer::Object3dInfo* icosphereMedium;
    VEngine::Renderer::Object3dInfo* icosphereHigh;


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

    int cascadeCounter = 0;

//#define PERFORMANCE_DEBUG
    double measurementStopwatch = 0.0;
    void measureTimeStart();
    void measureTimeEnd(std::string name);

    std::vector<std::tuple<glm::vec3, VEngine::Renderer::Object3dInfo*>> patchesLowPoly = {};
    std::vector<std::tuple<glm::vec3, VEngine::Renderer::Object3dInfo*>> patchesMediumPoly = {};
    std::vector<std::tuple<glm::vec3, VEngine::Renderer::Object3dInfo*>> patchesHighPoly = {};

    VEngine::Renderer::Object3dInfo* subdivide(VEngine::Renderer::Object3dInfo* info);
    std::vector<VEngine::Renderer::Object3dInfo*> splitTriangles(VEngine::Renderer::Object3dInfo* info);
};

