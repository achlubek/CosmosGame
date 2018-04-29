#include "stdafx.h"
#include "ModelsRenderer.h"
#include "SceneProvider.h"
#include "AbsGameContainer.h"
#include "TimeProvider.h"


ModelsRenderer::ModelsRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight)
    : vulkan(ivulkan), width(iwidth), height(iheight)
{
    modelMRTLayout = new VulkanDescriptorSetLayout(vulkan);
    modelMRTLayout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    modelMRTLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->compile();

    modelsDataLayout = new VulkanDescriptorSetLayout(vulkan);
    modelsDataLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT);
    modelsDataLayout->compile();

    modelsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 1024);

    modelsDataSet = modelsDataLayout->generateDescriptorSet();
    modelsDataSet->bindUniformBuffer(0, modelsDataBuffer);
    modelsDataSet->update();

    modelsAlbedoRoughnessImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    modelsNormalMetalnessImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    modelsDistanceImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    modelsDepthImage = new VulkanImage(vulkan, width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);


    auto shipvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-ship.vert.spv");
    auto shipfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-ship.frag.spv");

    modelsStage = new VulkanRenderStage(vulkan);
    modelsStage->setViewport(width, height);
    modelsStage->addShaderStage(shipvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    modelsStage->addShaderStage(shipfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    modelsStage->addDescriptorSetLayout(modelsDataLayout->layout);
    modelsStage->addDescriptorSetLayout(modelMRTLayout->layout);
    modelsStage->addOutputImage(modelsAlbedoRoughnessImage);
    modelsStage->addOutputImage(modelsNormalMetalnessImage);
    modelsStage->addOutputImage(modelsDistanceImage);
    modelsStage->addOutputImage(modelsDepthImage);
    modelsStage->cullFlags = 0;
    modelsStage->compile();
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

    bb.emplaceFloat32((float)AbsGameContainer::getInstance()->getTimeProvider()->getTime());
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
