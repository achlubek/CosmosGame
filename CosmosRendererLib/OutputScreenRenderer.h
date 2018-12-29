#pragma once
class OutputScreenRenderer
{
public:
    OutputScreenRenderer(VEngine::Renderer::ToolkitInterface* toolkit, int width, int height, VEngine::Renderer::ImageInterface * image, VEngine::Renderer::ImageInterface * uiImage);
    ~OutputScreenRenderer();
    void draw();
private:
    VEngine::Renderer::ToolkitInterface * toolkit;
    int width;
    int height;

    VEngine::Renderer::SwapChainOutputInterface* swapChainOutput{ nullptr };
    VEngine::Renderer::DescriptorSetLayoutInterface* layout{ nullptr };
    VEngine::Renderer::DescriptorSetInterface* set{ nullptr };
    VEngine::Renderer::RenderStageInterface* renderStage{ nullptr };

};

