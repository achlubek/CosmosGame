#include "stdafx.h"
#include "Model3d.h"
 

Model3d::Model3d(ToolkitInterface * toolkit, DescriptorSetLayoutInterface * descriptorSetLayout, std::string info3d_file, std::string albedo_image, 
    std::string normal_image, std::string roughness_image, std::string metalness_image, std::string emission_idle_image, std::string emission_powered_image,
    glm::dquat orientationCorrection, double modelScale)
    : toolkit(toolkit),
    info3d_file(info3d_file), 
    orientationCorrection(orientationCorrection),
    albedo_image(albedo_image),
    normal_image(normal_image),
    roughness_image(roughness_image),
    metalness_image(metalness_image),
    emission_idle_image(emission_idle_image),
    emission_powered_image(emission_powered_image),
    modelScale(modelScale)
{

    info3d = toolkit->getObject3dInfoFactory()->build(info3d_file);
    albedoImage = toolkit->getImageFactory()->build(albedo_image);
    normalImage = toolkit->getImageFactory()->build(normal_image);
    roughnessImage = toolkit->getImageFactory()->build(roughness_image);
    metalnessImage = toolkit->getImageFactory()->build(metalness_image);
    emissionIdleImage = toolkit->getImageFactory()->build(emission_idle_image);
    emissionPoweredImage = toolkit->getImageFactory()->build(emission_powered_image);

    descriptorSet = descriptorSetLayout->generateDescriptorSet();
    dataBuffer = toolkit->getBufferFactory()->build(VEngineBufferType::BufferTypeStorage, sizeof(float) * 1024);
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

void Model3d::draw(RenderStageInterface * stage, DescriptorSetInterface* celestialSet, glm::dvec3 position, glm::dquat orientation, double scale, int id, double emission)
{
    BinaryBufferBuilder bb2 = BinaryBufferBuilder();
    glm::mat4 shipmat = glm::mat4_cast(orientation * orientationCorrection);
    bb2.emplaceGeneric((unsigned char*)&shipmat, sizeof(shipmat));
    position *= scale;
    bb2.emplaceFloat32((float)(position).x);
    bb2.emplaceFloat32((float)(position).y);
    bb2.emplaceFloat32((float)(position).z);
    bb2.emplaceFloat32((float)scale * modelScale);
    bb2.emplaceInt32(id);
    bb2.emplaceInt32(id);
    bb2.emplaceInt32(id);
    bb2.emplaceInt32(id);
    bb2.emplaceFloat32(emission);
    bb2.emplaceFloat32(emission);
    bb2.emplaceFloat32(emission);
    bb2.emplaceFloat32(emission);
    void* data;
    dataBuffer->map(0, bb2.buffer.size(), &data);
    memcpy(data, bb2.getPointer(), bb2.buffer.size());
    dataBuffer->unmap();
    stage->setSet(0, celestialSet);
    stage->setSet(1, descriptorSet);
    stage->drawMesh(info3d, 1);
}
