#pragma once
class GalaxyContainer;
class StarsRenderer
{
public:
    StarsRenderer(VulkanToolkit* vulkan, 
        int width, int height, double scale,
        VulkanDescriptorSetLayout* rendererDataSetLayout,
        VulkanDescriptorSet* rendererDataSet,
        GalaxyContainer* galaxy);
    ~StarsRenderer();
    void draw(std::vector<VkSemaphore> waitSemaphores);
    void recompile();
    VulkanImage* getStarsImage();
    VkSemaphore getSignalSemaphore();
private:
    VulkanToolkit * vulkan;
    GalaxyContainer* galaxy;
    VulkanDescriptorSetLayout* rendererDataSetLayout;
    VulkanDescriptorSet* rendererDataSet;

    int width;
    int height;
    double scale;
    bool doesNeedRecording;

    VulkanDescriptorSetLayout* starsDataLayout{ nullptr };
    VulkanDescriptorSet* starsDataSet{ nullptr };
    VulkanRenderStage* starsStage{ nullptr };
    VulkanImage* starsImage;
    VulkanGenericBuffer* starsDataBuffer;
    Object3dInfo* cube3dInfo;

    void updateStarsBuffer();
    void createRenderStage();
};

