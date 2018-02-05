#pragma once

class VulkanRenderer;
class VulkanToolkit;
class SceneProvider;
class TimeProvider;
#include "GalaxyContainer.h" 
#include "InvokeQueue.h" 

class CosmosRenderer
{
public:
    CosmosRenderer(VulkanToolkit* ivulkan, TimeProvider* timeProvider, SceneProvider* sceneProvider, GalaxyContainer* galaxy, VulkanImage* overlayImage, int iwidth, int iheight);
    ~CosmosRenderer();
    void recompileShaders(bool deleteOld); 

    GalaxyContainer* galaxy;
    VulkanImage* overlayImage;
    AssetLoader assets;
    TimeProvider* timeProvider;

    Camera* internalCamera; 

    SceneProvider* sceneProvider;

    int lastPlanetId;

    int width{ 0 };
    int height{ 0 };

    VulkanToolkit* vulkan{ nullptr };
    VulkanRenderer* renderer{ nullptr };

    VulkanRenderStage* celestialStage{ nullptr };
    VulkanDescriptorSetLayout* rendererDataLayout{ nullptr };
    VulkanDescriptorSet* rendererDataSet{ nullptr };
    VulkanDescriptorSetLayout* starsDataLayout{ nullptr };
    VulkanDescriptorSet* starsDataSet{ nullptr };

    VulkanRenderStage* starsStage{ nullptr };
   // VulkanDescriptorSetLayout* starsLayout{ nullptr };
   // VulkanDescriptorSet* starsSet{ nullptr };

    VulkanRenderStage* modelsStage{ nullptr };

    VulkanRenderStage* combineStage{ nullptr };
    VulkanDescriptorSetLayout* combineLayout{ nullptr };
    VulkanDescriptorSet* combineSet{ nullptr };


    VulkanDescriptorSetLayout* modelMRTLayout{ nullptr };

    VulkanDescriptorSetLayout* celestialBodyDataSetLayout{ nullptr };
    VulkanDescriptorSetLayout* celestialBodyRenderSetLayout{ nullptr };
    VulkanComputeStage* celestialDataUpdateComputeStage;

    VulkanGenericBuffer* cameraDataBuffer;
    VulkanGenericBuffer* starsDataBuffer;
    VulkanGenericBuffer* planetsDataBuffer;
    VulkanGenericBuffer* moonsDataBuffer;

    VulkanImage* celestialImage;
    VulkanImage* starsImage;
    //VulkanImage* cosmosImage;
    VulkanImage* modelsResultImage;
    VulkanImage* modelsDepthImage; 

    VulkanImage* planetTerrainHeightImage;
    VulkanImage* planetTerrainColorImage;
    VulkanImage* planetAtmosphereFlunctuationsImage;

    Object3dInfo* cube3dInfo; 

    const double scale = 0.01;

    glm::dvec3 observerCameraPosition;
    double closestSurfaceDistance;

    void* starsDataBufferPointer;
    void* planetsDataBufferPointer;
    void* moonsDataBufferPointer;

    void mapBuffers();
    void unmapBuffers();

    void updateStarsBuffer();
    
    void updateCameraBuffer(Camera* cam, glm::dvec3 observerPosition);
    void draw();

    void onClosestStarChange(GeneratedStarInfo star);
    void onClosestPlanetChange(CelestialBody planet);

    std::vector<RenderedCelestialBody*> renderablePlanets;
    std::vector<RenderedCelestialBody*> renderableMoons;

    InvokeQueue updatingSafetyQueue;

    volatile bool readyForDrawing = false;
};

