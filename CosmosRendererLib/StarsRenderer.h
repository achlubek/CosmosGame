#pragma once
class GalaxyContainer;
class StarsRenderer
{
public:
    StarsRenderer(VulkanToolkit* vulkan, 
        int width, int height, double scale,
        VulkanDescriptorSet* rendererDataSet, 
        AssetLoader* assetLoader, 
        GalaxyContainer* galaxy);
    ~StarsRenderer();
    void draw();
    VulkanImage* getStarsImage();
private:
    VulkanToolkit * vulkan;
    AssetLoader* assetLoader;
    GalaxyContainer* galaxy;

    int width;
    int height;
    double scale;
    bool doesNeedsRecording;

    VulkanDescriptorSetLayout* starsDataLayout{ nullptr };
    VulkanDescriptorSet* starsDataSet{ nullptr };
    VulkanRenderStage* starsStage{ nullptr };
    VulkanImage* starsImage;
    VulkanGenericBuffer* starsDataBuffer;
    Object3dInfo* cube3dInfo;

    void updateStarsBuffer();
};

