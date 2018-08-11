#include "stdafx.h"
#include "ModelsRenderer.h"
#include "SceneProvider.h"
#include "AbsGameContainer.h"
#include "TimeProvider.h"
#include "AbsGameStage.h"

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

    modelsDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 1024);

    modelsDataSet = modelsDataLayout->generateDescriptorSet();
    modelsDataSet->bindBuffer(0, modelsDataBuffer);

    modelsAlbedoRoughnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA8unorm, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsNormalMetalnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsDistanceImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsIDImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32u, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    modelsDepthImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth);


    auto shipvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-ship.vert.spv");
    auto shipfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-ship.frag.spv");

    auto modelsStage = vulkan->getVulkanRenderStageFactory()->build(width, height, { shipvert, shipfrag }, { modelsDataLayout, modelMRTLayout }, {
        modelsAlbedoRoughnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsNormalMetalnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsDistanceImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsIDImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        modelsDepthImage->getAttachment(VulkanAttachmentBlending::None)
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
}

void ModelsRenderer::draw(SceneProvider * scene)
{
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

VulkanImage * ModelsRenderer::getNormalMetalnessImage()
{
    return modelsNormalMetalnessImage;
}

VulkanImage * ModelsRenderer::getDistanceImage()
{
    return modelsDistanceImage;
}

VulkanDescriptorSetLayout * ModelsRenderer::getModelMRTLayout()
{
    return modelMRTLayout;
}

void ModelsRenderer::updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition)
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    auto cursor = AbsGameContainer::getInstance()->getVulkanToolkit()->getMouse()->getCursorPosition();
    xpos = std::get<0>(cursor);
    ypos = std::get<1>(cursor);

    glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    glm::mat4 vpmatrix = clip * camera->projectionMatrix * camera->transformation->getInverseWorldTransform();

    glm::mat4 cameraViewMatrix = camera->transformation->getInverseWorldTransform();
    glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
    glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
    camera->cone->update(inverse(rpmatrix));

    bb.emplaceFloat32((float)AbsGameContainer::getInstance()->getCurrentStage()->getTimeProvider()->getTime());
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)xpos / (float)width);
    bb.emplaceFloat32((float)ypos / (float)height);
    bb.emplaceGeneric((unsigned char*)&rpmatrix, sizeof(rpmatrix));

    glm::vec3 newcamerapos = glm::vec3(observerPosition * renderingScale);
    bb.emplaceGeneric((unsigned char*)&newcamerapos, sizeof(camera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);

    bb.emplaceGeneric((unsigned char*)&(camera->cone->leftBottom), sizeof(camera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(camera->cone->rightBottom - camera->cone->leftBottom), sizeof(camera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(camera->cone->leftTop - camera->cone->leftBottom), sizeof(camera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)width);
    bb.emplaceFloat32((float)height);
    bb.emplaceFloat32((float)renderingScale);
    bb.emplaceFloat32((float)renderingScale);

    void* data;
    modelsDataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    modelsDataBuffer->unmap();
}
