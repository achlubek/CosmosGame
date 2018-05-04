#pragma once
class OutputScreenRenderer
{
public:
    OutputScreenRenderer(VulkanToolkit* vulkan, int width, int height);
    ~OutputScreenRenderer();
    void draw(VulkanImage* image, VulkanImage* uiImage);
private:
    VulkanImage * lastImage{ nullptr };
    VulkanImage * lastUiImage{ nullptr };
    void rebuildDescriptorSet(VulkanImage* image, VulkanImage* uiImage);

    VulkanToolkit * vulkan;
    int width;
    int height;

    VulkanRenderer* renderer{ nullptr };
    VulkanDescriptorSetLayout* layout{ nullptr };
    VulkanDescriptorSet* set{ nullptr };
    VulkanRenderStage* renderStage{ nullptr };

};

