#include "stdafx.h"
#include "ParticlesRenderer.h"
#include "AbsGameContainer.h"
#include "TimeProvider.h"
#include "AbsGameStage.h"
#include "ParticleSystem.h"
#include <Input/Mouse.h>

ParticlesRenderer::ParticlesRenderer(VulkanToolkit * vulkan, int width, int height, VulkanImage * mrtDistanceTexture)
    : vulkan(vulkan), width(width), height(height), mrtDistanceTexture(mrtDistanceTexture), particleSystems({})
{
    particleLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    particleLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    particleLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    rendererDataLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    rendererDataLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);
    rendererDataLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    rendererDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 1024);

    rendererDataSet = rendererDataLayout->generateDescriptorSet();
    rendererDataSet->bindBuffer(0, rendererDataBuffer);
    rendererDataSet->bindImageViewSampler(1, mrtDistanceTexture);

    resultImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    auto shipvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "particle-renderer.vert.spv");
    auto shipfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "particle-renderer.frag.spv");

    renderStage = vulkan->getVulkanRenderStageFactory()->build(width, height, { shipvert, shipfrag }, { rendererDataLayout, particleLayout },
    {
        resultImage->getAttachment(VulkanAttachmentBlending::Additive, true, { { 0.0f, 0.0f, 0.0f, 1.0f } }),
    });
}

#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}
ParticlesRenderer::~ParticlesRenderer()
{
    safedelete(renderStage);
    safedelete(resultImage);
    safedelete(rendererDataSet);
    safedelete(rendererDataBuffer);
    safedelete(rendererDataLayout);
    safedelete(particleLayout);
}

void ParticlesRenderer::update(double elapsed)
{
    for (int i = 0; i < particleSystems.size(); i++) {
        particleSystems[i]->update(elapsed);
    }
}

void ParticlesRenderer::draw()
{
    renderStage->beginDrawing();
    for (int i = 0; i < particleSystems.size(); i++) {
        if (particleSystems[i]->getCount() == 0) break;
        renderStage->setSets({ rendererDataSet, particleSystems[i]->getSet() });
        renderStage->drawMesh(AbsGameContainer::getInstance()->getVulkanToolkit()->getObject3dInfoFactory()->getFullScreenQuad(), particleSystems[i]->getCount());
    }
    renderStage->endDrawing();
    renderStage->submitNoSemaphores({});
}

void ParticlesRenderer::setRenderingScale(double irenderingScale)
{
    renderingScale = irenderingScale;
}

VulkanDescriptorSetLayout * ParticlesRenderer::getParticleLayout()
{
    return particleLayout;
}

VulkanImage * ParticlesRenderer::getResultImage()
{
    return resultImage;
}

void ParticlesRenderer::updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition)
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
    rendererDataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    rendererDataBuffer->unmap();

    for (int i = 0; i < particleSystems.size(); i++) {
        particleSystems[i]->updateBuffers(observerPosition);
    }
}

void ParticlesRenderer::registerParticleSystem(ParticleSystem * system)
{
    particleSystems.push_back(system);
}
