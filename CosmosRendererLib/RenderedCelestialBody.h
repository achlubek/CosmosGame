#pragma once
class CelestialBody;
class RenderedCelestialBody
{
public:
    RenderedCelestialBody(ToolkitInterface* toolkit, CelestialBody body, 
        DescriptorSetLayoutInterface* dataSetLayout, DescriptorSetLayoutInterface* renderSetLayout,
        DescriptorSetLayoutInterface* celestialBodySurfaceSetLayout,
        DescriptorSetLayoutInterface* celestialBodyRaycastUniqueSetLayout,
        ImageInterface* surfaceRenderedAlbedoRoughnessImage,
        ImageInterface* surfaceRenderedNormalMetalnessImage,
        ImageInterface* surfaceRenderedEmissionImage,
        ImageInterface* surfaceRenderedDistanceImage);
    ~RenderedCelestialBody();
    void updateRaycasts(uint32_t raycastPointsCount, DescriptorSetInterface* celestialBodyRaycastSharedSet, ComputeStageInterface* stage);
    void updateData(ComputeStageInterface* stage);
    void draw(RenderStageInterface* stage, DescriptorSetInterface* rendererDataSet, Object3dInfoInterface* info3d);
    void drawSurface(RenderStageInterface* stage, DescriptorSetInterface* rendererDataSet, Object3dInfoInterface* info3d);
    void updateBuffer(glm::dvec3 observerPosition, double scale, double time);
    double getDistance(glm::dvec3 position, double at_time);
    void resizeDataImages(int lowFreqWidth, int lowFreqHeight, int hiFreqWidth, int hiFreqHeight);
    bool needsDataUpdate();
    CelestialRenderMethod getRenderMethod();
    double getRadius();
    std::vector<glm::dvec4> getRaycastResults(int32_t count);

    DescriptorSetInterface* renderSurfaceSet;
    CelestialBody body;
private:

    int lowFreqWidth = 0;
    int lowFreqHeight = 0;

    int hiFreqWidth = 0;
    int hiFreqHeight = 0;

    bool initialized = false;
    bool needsUpdate = true;

    ToolkitInterface * toolkit;


    ImageInterface* heightMapImage = nullptr;
    ImageInterface* baseColorImage = nullptr;
    ImageInterface* cloudsImage = nullptr;
    
    ImageInterface* surfaceRenderedAlbedoRoughnessImage;
    ImageInterface* surfaceRenderedNormalMetalnessImage;
    ImageInterface* surfaceRenderedEmissionImage;
    ImageInterface* surfaceRenderedDistanceImage;

    GenericBufferInterface* dataBuffer;

    DescriptorSetInterface* dataSet;

    DescriptorSetInterface* renderSet;

    DescriptorSetInterface* celestialBodyRaycastUniqueSet;

    GenericBufferInterface* raycastResultsBuffer;
};

