#pragma once
class OutputScreenRenderer
{
public:
    OutputScreenRenderer(VulkanToolkit* vulkan, int width, int height, VulkanImage * image, VulkanImage * uiImage);
    ~OutputScreenRenderer();
    void draw();
private:
    VulkanToolkit * vulkan;
    int width;
    int height;

    VulkanRenderer* renderer{ nullptr };
    VulkanDescriptorSetLayout* layout{ nullptr };
    VulkanDescriptorSet* set{ nullptr };
    VulkanRenderStage* renderStage{ nullptr };

};

