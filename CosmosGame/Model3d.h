#pragma once
class Object3dInfo;
class VulkanImage;
class VulkanToolkit;
class VulkanDescriptorSet;
class VulkanRenderStage;
class Model3d
{
public:
    Model3d(VulkanToolkit* vulkan, VulkanDescriptorSetLayout* descriptorSetLayout, std::string mediakey);
    ~Model3d();
    void draw(VulkanRenderStage* stage, VulkanDescriptorSet* celestialSet, glm::dvec3 position, glm::dquat orientation);
private:
    VulkanToolkit* vulkan;
    Object3dInfo* info3d;
    VulkanGenericBuffer* dataBuffer;
    VulkanImage* albedoImage;
    VulkanImage* normalImage;
    VulkanImage* roughnessImage;
    VulkanImage* metalnessImage;
    VulkanImage* emissionIdleImage;
    VulkanImage* emissionPoweredImage;
    VulkanDescriptorSet* descriptorSet;
};

