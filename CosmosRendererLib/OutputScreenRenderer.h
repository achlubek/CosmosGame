#pragma once
class OutputScreenRenderer
{
public:
    OutputScreenRenderer(VEngine::Renderer::VulkanToolkit* vulkan, int width, int height, VEngine::Renderer::VulkanImage * image, VEngine::Renderer::VulkanImage * uiImage);
    ~OutputScreenRenderer();
    void draw();
private:
    VEngine::Renderer::VulkanToolkit * vulkan;
    int width;
    int height;

    VEngine::Renderer::VulkanSwapChainOutput* swapChainOutput{ nullptr };
    VEngine::Renderer::VulkanDescriptorSetLayout* layout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSet* set{ nullptr };
    VEngine::Renderer::VulkanRenderStage* renderStage{ nullptr };

};

