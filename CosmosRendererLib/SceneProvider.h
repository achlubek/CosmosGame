#pragma once
class VulkanRenderStage;
class VulkanDescriptorSet;
class SceneProvider
{
public:
    virtual void drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set) = 0;
};

