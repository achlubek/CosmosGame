#pragma once
class CelestialBody;
class RenderedCelestialBody
{
public:
    RenderedCelestialBody(VulkanToolkit* toolkit, CelestialBody body, 
        VulkanDescriptorSetLayout* dataSetLayout, VulkanDescriptorSetLayout* shadowMapSetLayout, VulkanDescriptorSetLayout* renderSetLayout,
        VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout,
        VulkanDescriptorSetLayout* celestialBodyWaterSetLayout,
        VulkanImage* surfaceRenderedAlbedoRoughnessImage,
        VulkanImage* surfaceRenderedNormalMetalnessImage,
        VulkanImage* surfaceRenderedDistanceImage,
        VulkanImage* waterRenderedNormalMetalnessImage,
        VulkanImage* waterRenderedDistanceImage);
    ~RenderedCelestialBody();
    void updateData(VulkanComputeStage* stage);
    void draw(VulkanRenderStage* stage, VulkanDescriptorSet* rendererDataSet, VulkanDescriptorSet* shadowMapsCollectionSet, Object3dInfo* info3d);
    void drawSurface(VulkanRenderStage* stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo* info3d);
    void drawWater(VulkanRenderStage* stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo* info3d);
    void updateBuffer(glm::dvec3 observerPosition, double scale, double time);
    double getDistance(glm::dvec3 position, double at_time);
    void resizeDataImages(int lowFreqWidth, int lowFreqHeight, int hiFreqWidth, int hiFreqHeight);
    bool needsDataUpdate();
    CelestialRenderMethod getRenderMethod();
    double getRadius();

    VulkanDescriptorSet* renderSurfaceSet;
    VulkanDescriptorSet* shadowMapSet;
    VulkanDescriptorSet* renderWaterSet;
    CelestialBody body;
private:

    int lowFreqWidth = 0;
    int lowFreqHeight = 0;

    int hiFreqWidth = 0;
    int hiFreqHeight = 0;

    bool initialized = false;
    bool needsUpdate = true;

    int shadowMapWidthOffset = 0;

    VulkanToolkit * toolkit;


    VulkanImage* heightMapImage = nullptr;
    VulkanImage* baseColorImage = nullptr;
    VulkanImage* cloudsImage = nullptr;

    VulkanImage* shadowMapImage = nullptr;    
    
    VulkanImage* surfaceRenderedAlbedoRoughnessImage;
    VulkanImage* surfaceRenderedNormalMetalnessImage;
    VulkanImage* surfaceRenderedDistanceImage;
    VulkanImage* waterRenderedNormalMetalnessImage;
    VulkanImage* waterRenderedDistanceImage;

    VulkanGenericBuffer* dataBuffer;

    VulkanDescriptorSet* dataSet;

    VulkanDescriptorSet* renderSet;

};

