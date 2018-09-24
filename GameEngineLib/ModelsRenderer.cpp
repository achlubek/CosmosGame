#include "stdafx.h"
#include "ModelsRenderer.h"

using namespace VEngine::Renderer;

ModelsRenderer::ModelsRenderer(VulkanToolkit* vulkan, int iwidth, int iheight)
    : vulkan(vulkan), width(iwidth), height(iheight)
{
    modelMRTLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    modelMRTLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    modelMRTLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    modelMRTLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    modelMRTLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    modelMRTLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    modelMRTLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    modelMRTLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);

    modelsDataLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    modelsDataLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);
    modelsDataLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);

    modelsDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 1024);
    sunlightDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 1024);

    modelsDataSet = modelsDataLayout->generateDescriptorSet();
    modelsDataSet->bindBuffer(0, modelsDataBuffer);
    modelsDataSet->bindBuffer(1, sunlightDataBuffer);

    modelsAlbedoRoughnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA8unorm, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsEmissionImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsNormalMetalnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsDistanceImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsShadowDistanceImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsIDImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32u, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsDepthImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth);

    auto shipvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-ship.vert.spv");
    auto shipfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-ship.frag.spv");

    modelsStage = vulkan->getVulkanRenderStageFactory()->build(width, height, { shipvert, shipfrag }, { modelsDataLayout, modelMRTLayout }, {
        modelsAlbedoRoughnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsEmissionImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsNormalMetalnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsDistanceImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsIDImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsDepthImage->getAttachment(VulkanAttachmentBlending::None)
    });

    auto shipvertshadow = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-ship-shadow.vert.spv");
    auto shipfragshadow = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-ship-shadow.frag.spv");

    modelsShadowStage = vulkan->getVulkanRenderStageFactory()->build(width, height, { shipvertshadow, shipfragshadow }, { modelsDataLayout, modelMRTLayout }, {
        modelsShadowDistanceImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsDepthImage->getAttachment(VulkanAttachmentBlending::None, true)
    });

}


#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}
ModelsRenderer::~ModelsRenderer()
{
    safedelete(modelsStage);
    safedelete(modelsDepthImage);
    safedelete(modelsDistanceImage);
    safedelete(modelsNormalMetalnessImage);
    safedelete(modelsAlbedoRoughnessImage);
    safedelete(modelsDataSet);
    safedelete(modelsDataLayout);
    safedelete(modelMRTLayout);
    safedelete(sunlightDataBuffer);
    safedelete(modelsDataBuffer);
}

void ModelsRenderer::draw(SceneProvider * scene)
{
    modelsShadowStage->beginDrawing();

    scene->drawDrawableObjects(modelsShadowStage, modelsDataSet, renderingScale);

    modelsShadowStage->endDrawing();
    modelsShadowStage->submitNoSemaphores({});

    modelsStage->beginDrawing();

    scene->drawDrawableObjects(modelsStage, modelsDataSet, renderingScale);

    modelsStage->endDrawing();
    modelsStage->submitNoSemaphores({});
}

void ModelsRenderer::setRenderingScale(double irenderingScale)
{
    renderingScale = irenderingScale;
}

VulkanImage * ModelsRenderer::getAlbedoRoughnessImage()
{
    return modelsAlbedoRoughnessImage;
}

VEngine::Renderer::VulkanImage * ModelsRenderer::getEmissionImage()
{
    return modelsEmissionImage;
}

VulkanImage * ModelsRenderer::getNormalMetalnessImage()
{
    return modelsNormalMetalnessImage;
}

VulkanImage * ModelsRenderer::getDistanceImage()
{
    return modelsDistanceImage;
}

VEngine::Renderer::VulkanImage * ModelsRenderer::getShadowDistanceImage()
{
    return modelsShadowDistanceImage;
}

VulkanDescriptorSetLayout * ModelsRenderer::getModelMRTLayout()
{
    return modelMRTLayout;
}

void ModelsRenderer::updateCameraBuffer(Camera * camera, glm::mat4 starLookAtThisMatrix)
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    auto cursor = AbsGameContainer::getInstance()->getVulkanToolkit()->getMouse()->getCursorPosition();
    xpos = std::get<0>(cursor);
    ypos = std::get<1>(cursor);

    glm::mat4 rpmatrix = camera->getRotationProjectionMatrix();
    auto cone = camera->getFrustumCone();

    bb.emplaceFloat32((float)AbsGameContainer::getInstance()->getCurrentStage()->getTimeProvider()->getTime());
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)xpos / (float)width);
    bb.emplaceFloat32((float)ypos / (float)height);
    bb.emplaceGeneric((unsigned char*)&rpmatrix, sizeof(rpmatrix));

    glm::vec3 newcamerapos = glm::vec3(camera->getPosition() * renderingScale);
    bb.emplaceGeneric((unsigned char*)&newcamerapos, sizeof(cone->leftBottom));
    bb.emplaceFloat32(0.0f);

    bb.emplaceGeneric((unsigned char*)&(cone->leftBottom), sizeof(cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(cone->rightBottom - cone->leftBottom), sizeof(cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(cone->leftTop - cone->leftBottom), sizeof(cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)width);
    bb.emplaceFloat32((float)height);
    bb.emplaceFloat32((float)renderingScale);
    bb.emplaceFloat32((float)renderingScale);

    void* data;
    modelsDataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    modelsDataBuffer->unmap();

    VulkanBinaryBufferBuilder bb2 = VulkanBinaryBufferBuilder();
    bb2.emplaceGeneric((unsigned char*)&starLookAtThisMatrix, sizeof(starLookAtThisMatrix));

    void* data2;
    sunlightDataBuffer->map(0, bb2.buffer.size(), &data2);
    memcpy(data2, bb2.getPointer(), bb2.buffer.size());
    sunlightDataBuffer->unmap();

}
