#include "stdafx.h"
#include "ParticlesRenderer.h"
#include "AbsGameContainer.h"
#include "TimeProvider.h"
#include "AbsGameStage.h"

ParticlesRenderer::ParticlesRenderer(VulkanToolkit * vulkan, int width, int height, VulkanImage * mrtDistanceTexture)
    : vulkan(vulkan), width(width), height(height), mrtDistanceTexture(mrtDistanceTexture)
{
    particleLayout = new VulkanDescriptorSetLayout(vulkan);
    particleLayout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    particleLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    particleLayout->compile();

    rendererDataLayout = new VulkanDescriptorSetLayout(vulkan);
    rendererDataLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT);
    rendererDataLayout->compile();

    rendererDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 1024);

    rendererDataSet = rendererDataLayout->generateDescriptorSet();
    rendererDataSet->bindUniformBuffer(0, rendererDataBuffer);
    rendererDataSet->update();

    resultImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);
    
    auto shipvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/particle-renderer.vert.spv");
    auto shipfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/particle-renderer.frag.spv");

    renderStage = new VulkanRenderStage(vulkan);
    renderStage->setViewport(width, height);
    renderStage->addShaderStage(shipvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    renderStage->addShaderStage(shipfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    renderStage->addDescriptorSetLayout(particleLayout->layout);
    renderStage->addDescriptorSetLayout(rendererDataSet->layout);
    renderStage->addOutputImage(resultImage);
    renderStage->cullFlags = 0;
    renderStage->compile();
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

void ParticlesRenderer::draw()
{
}

void ParticlesRenderer::setRenderingScale(double renderingScale)
{
}

VulkanDescriptorSetLayout * ParticlesRenderer::getParticleLayout()
{
    return nullptr;
}

void ParticlesRenderer::updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition)
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    glfwGetCursorPos(vulkan->window, &xpos, &ypos);

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
}
