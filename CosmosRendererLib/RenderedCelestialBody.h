#pragma once
class CelestialBody;
class RenderedCelestialBody
{
public:
    RenderedCelestialBody(VulkanToolkit* toolkit, CelestialBody body, 
        VulkanDescriptorSetLayout* dataSetLayout, VulkanDescriptorSetLayout* renderSetLayout,
        VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout,
        VulkanDescriptorSetLayout* celestialBodyRaycastUniqueSetLayout,
        VulkanImage* surfaceRenderedAlbedoRoughnessImage,
        VulkanImage* surfaceRenderedNormalMetalnessImage,
        VulkanImage* surfaceRenderedEmissionImage,
        VulkanImage* surfaceRenderedDistanceImage);
    ~RenderedCelestialBody();
    void updateRaycasts(uint32_t raycastPointsCount, VulkanDescriptorSet* celestialBodyRaycastSharedSet, VulkanComputeStage* stage);
    void updateData(VulkanComputeStage* stage);
    void draw(VulkanRenderStage* stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo* info3d);
    void drawSurface(VulkanRenderStage* stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo* info3d);
    void updateBuffer(glm::dvec3 observerPosition, double scale, double time);
    double getDistance(glm::dvec3 position, double at_time);
    void resizeDataImages(int lowFreqWidth, int lowFreqHeight, int hiFreqWidth, int hiFreqHeight);
    bool needsDataUpdate();
    CelestialRenderMethod getRenderMethod();
    double getRadius();
    std::vector<glm::dvec4> getRaycastResults(int32_t count);

    VulkanDescriptorSet* renderSurfaceSet;
    CelestialBody body;
private:

    int lowFreqWidth = 0;
    int lowFreqHeight = 0;

    int hiFreqWidth = 0;
    int hiFreqHeight = 0;

    bool initialized = false;
    bool needsUpdate = true;

    VulkanToolkit * toolkit;


    VulkanImage* heightMapImage = nullptr;
    VulkanImage* baseColorImage = nullptr;
    VulkanImage* cloudsImage = nullptr;
    
    VulkanImage* surfaceRenderedAlbedoRoughnessImage;
    VulkanImage* surfaceRenderedNormalMetalnessImage;
    VulkanImage* surfaceRenderedEmissionImage;
    VulkanImage* surfaceRenderedDistanceImage;

    VulkanGenericBuffer* dataBuffer;

    VulkanDescriptorSet* dataSet;

    VulkanDescriptorSet* renderSet;

    VulkanDescriptorSet* celestialBodyRaycastUniqueSet;

    VulkanGenericBuffer* raycastResultsBuffer;
};

