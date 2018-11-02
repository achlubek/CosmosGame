#pragma once
class SceneProvider
{
public:
    virtual void drawDrawableObjects(VEngine::Renderer::VulkanRenderStage* stage, VEngine::Renderer::VulkanDescriptorSet* set, double scale) = 0;
};

