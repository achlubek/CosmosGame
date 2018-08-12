#pragma once
class GalaxyContainer;
class StarsRenderer
{
public:
    StarsRenderer(VulkanToolkit* vulkan, 
        int width, int height, double scale,
        VulkanDescriptorSet* rendererDataSet, 
        GalaxyContainer* galaxy);
    ~StarsRenderer();
    void draw();
    void recompile();
    VulkanImage* getStarsImage();
private:
    VulkanToolkit * vulkan;
    GalaxyContainer* galaxy;
    VulkanDescriptorSet* rendererDataSet;

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
    void createRenderStage();
};

