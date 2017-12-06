#include "stdafx.h"
#include "INIReader.h"
#include "Model3d.h"
#include "VulkanToolkit.h"
#include "AssetLoader.h"


Model3d::Model3d(VulkanToolkit* ivulkan, VulkanDescriptorSetLayout* descriptorSetLayout, std::string mediakey)
    : vulkan(ivulkan)
{
    INIReader reader = INIReader(mediakey);
    AssetLoader assets = AssetLoader(vulkan);
    info3d = assets.loadObject3dInfoFile(reader.gets("model"));
    albedoImage = assets.loadTextureFile(reader.gets("albedo"));
    normalImage = assets.loadTextureFile(reader.gets("normal"));
    roughnessImage = assets.loadTextureFile(reader.gets("roughness"));
    metalnessImage = assets.loadTextureFile(reader.gets("metalness"));
    emissionIdleImage = assets.loadTextureFile(reader.gets("emission_idle"));
    emissionPoweredImage = assets.loadTextureFile(reader.gets("emission_powered"));
     
    descriptorSet = descriptorSetLayout->generateDescriptorSet();
    dataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024);
    descriptorSet->bindStorageBuffer(0, dataBuffer);
    descriptorSet->bindImageViewSampler(1, albedoImage);
    descriptorSet->bindImageViewSampler(2, normalImage);
    descriptorSet->bindImageViewSampler(3, roughnessImage);
    descriptorSet->bindImageViewSampler(4, metalnessImage);
    descriptorSet->bindImageViewSampler(5, emissionIdleImage);
    descriptorSet->bindImageViewSampler(6, emissionPoweredImage);
    descriptorSet->update();

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

void Model3d::draw(VulkanRenderStage * stage, VulkanDescriptorSet* celestialSet, glm::dvec3 position, glm::dquat orientation)
{
    VulkanBinaryBufferBuilder bb2 = VulkanBinaryBufferBuilder();
    glm::mat4 shipmat = glm::mat4_cast(orientation);
    bb2.emplaceGeneric((unsigned char*)&shipmat, sizeof(shipmat));
    bb2.emplaceFloat32((float)(position).x);
    bb2.emplaceFloat32((float)(position).y);
    bb2.emplaceFloat32((float)(position).z);
    bb2.emplaceFloat32(0.0f);
    void* data;
    dataBuffer->map(0, bb2.buffer.size(), &data);
    memcpy(data, bb2.getPointer(), bb2.buffer.size());
    dataBuffer->unmap();
    stage->setSets({ celestialSet, descriptorSet });
    stage->drawMesh(info3d, 1);
}
