#pragma once
class SceneProvider
{
public:
    virtual void drawDrawableObjects(VEngine::Renderer::RenderStageInterface* stage, VEngine::Renderer::DescriptorSetInterface* set, double scale) = 0;
};

