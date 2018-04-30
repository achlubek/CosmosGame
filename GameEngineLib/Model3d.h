#pragma once
class Object3dInfo;
class VulkanImage;
class VulkanToolkit;
class VulkanDescriptorSet;
class VulkanRenderStage;
class Model3d
{
public: 
    Model3d(VulkanToolkit* vulkan, VulkanDescriptorSetLayout* descriptorSetLayout, std::string info3d_file,
        std::string albedo_image, std::string normal_image, std::string roughness_image, std::string metalness_image, std::string emission_idle_image, std::string emission_powered_image,
        glm::dquat orientationCorrection);
    ~Model3d();
    void draw(VulkanRenderStage* stage, VulkanDescriptorSet* celestialSet, glm::dvec3 position, glm::dquat orientation, double scale, int id);
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
    glm::dquat orientationCorrection;
};

