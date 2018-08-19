#include "stdafx.h"
#include "INIReader.h"
#include "Model3d.h"
#include "AbsGameContainer.h"
 

Model3d::Model3d(VulkanToolkit * vulkan, VulkanDescriptorSetLayout * descriptorSetLayout, std::string info3d_file, std::string albedo_image, 
    std::string normal_image, std::string roughness_image, std::string metalness_image, std::string emission_idle_image, std::string emission_powered_image,
    glm::dquat iorientationCorrection)
    : vulkan(vulkan)
{

    orientationCorrection = iorientationCorrection;
    info3d = vulkan->getObject3dInfoFactory()->build(info3d_file);
    albedoImage = vulkan->getVulkanImageFactory()->build(albedo_image);
    normalImage = vulkan->getVulkanImageFactory()->build(normal_image);
    roughnessImage = vulkan->getVulkanImageFactory()->build(roughness_image);
    metalnessImage = vulkan->getVulkanImageFactory()->build(metalness_image);
    emissionIdleImage = vulkan->getVulkanImageFactory()->build(emission_idle_image);
    emissionPoweredImage = vulkan->getVulkanImageFactory()->build(emission_powered_image);

    descriptorSet = descriptorSetLayout->generateDescriptorSet();
    dataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, sizeof(float) * 1024);
    descriptorSet->bindBuffer(0, dataBuffer);
    descriptorSet->bindImageViewSampler(1, albedoImage);
    descriptorSet->bindImageViewSampler(2, normalImage);
    descriptorSet->bindImageViewSampler(3, roughnessImage);
    descriptorSet->bindImageViewSampler(4, metalnessImage);
    descriptorSet->bindImageViewSampler(5, emissionIdleImage);
    descriptorSet->bindImageViewSampler(6, emissionPoweredImage);
}


Model3d::~Model3d()
{
    delete descriptorSet;
    delete dataBuffer;
    delete info3d;
    delete albedoImage;
    delete normalImage;
    delete roughnessImage;
    delete metalnessImage;
    delete emissionIdleImage;
    delete emissionPoweredImage;
}

void Model3d::draw(VulkanRenderStage * stage, VulkanDescriptorSet* celestialSet, glm::dvec3 position, glm::dquat orientation, double scale, int id)
{
    VulkanBinaryBufferBuilder bb2 = VulkanBinaryBufferBuilder();
    glm::mat4 shipmat = glm::mat4_cast(orientation * orientationCorrection);
    bb2.emplaceGeneric((unsigned char*)&shipmat, sizeof(shipmat));
    position *= scale;
    bb2.emplaceFloat32((float)(position).x);
    bb2.emplaceFloat32((float)(position).y);
    bb2.emplaceFloat32((float)(position).z);
    bb2.emplaceFloat32((float)scale);
    bb2.emplaceInt32(id);
    bb2.emplaceInt32(id);
    bb2.emplaceInt32(id);
    bb2.emplaceInt32(id);
    void* data;
    dataBuffer->map(0, bb2.buffer.size(), &data);
    memcpy(data, bb2.getPointer(), bb2.buffer.size());
    dataBuffer->unmap();
    stage->setSets({ celestialSet, descriptorSet });
    stage->drawMesh(info3d, 1);
}
