#include "stdafx.h"
#include "CelestialBody.h"
#include "RenderedCelestialBody.h"


RenderedCelestialBody::RenderedCelestialBody(VulkanToolkit* itoolkit, CelestialBody ibody, VulkanDescriptorSetLayout* dataSetLayout, VulkanDescriptorSetLayout* renderSetLayout)
    : toolkit(itoolkit), body(ibody)
{
    heightMapImage = new VulkanImage(toolkit, TEXTURES_WIDTH, TEXTURES_HEIGHT, VK_FORMAT_R16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);
    
    baseColorImage = new VulkanImage(toolkit, TEXTURES_WIDTH, TEXTURES_HEIGHT, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);
    
    cloudsImage = new VulkanImage(toolkit, TEXTURES_WIDTH, TEXTURES_HEIGHT, VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    dataBuffer = new VulkanGenericBuffer(toolkit, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 65535);

    dataSet = dataSetLayout->generateDescriptorSet();
    dataSet->bindUniformBuffer(0, dataBuffer);
    dataSet->bindImageStorage(1, heightMapImage);
    dataSet->bindImageStorage(2, baseColorImage);
    dataSet->bindImageStorage(3, cloudsImage);
    dataSet->update();

    renderSet = renderSetLayout->generateDescriptorSet();
    renderSet->bindUniformBuffer(0, dataBuffer);
    renderSet->bindImageViewSampler(1, heightMapImage);
    renderSet->bindImageViewSampler(2, baseColorImage);
    renderSet->bindImageViewSampler(3, cloudsImage);
    renderSet->update();
}


#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}
RenderedCelestialBody::~RenderedCelestialBody()
{
    safedelete(renderSet);
    safedelete(dataSet);
    safedelete(dataBuffer);
    safedelete(cloudsImage);
    safedelete(baseColorImage);
    safedelete(heightMapImage);
}

void RenderedCelestialBody::updateData(VulkanComputeStage * stage)
{
    stage->dispatch({ dataSet }, TEXTURES_WIDTH, TEXTURES_HEIGHT, 1);
}

void RenderedCelestialBody::draw(VulkanRenderStage * stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo * info3d)
{
    stage->setSets({ rendererDataSet, renderSet });
    stage->drawMesh(info3d, 1);
}

void RenderedCelestialBody::updateBuffer(glm::dvec3 observerPosition, double scale, double time)
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    bb.emplaceFloat32(time);
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32(0.0f);

    auto bodyPosition = body.getPosition(0) - observerPosition;

    bb.emplaceInt32((int)body.getRenderMethod());
    bb.emplaceInt32(0);
    bb.emplaceInt32(0);
    bb.emplaceInt32(0);

    bb.emplaceFloat32(bodyPosition.x * scale);
    bb.emplaceFloat32(bodyPosition.y * scale);
    bb.emplaceFloat32(bodyPosition.z * scale);
    bb.emplaceFloat32(body.radius * scale);

    bb.emplaceFloat32(body.preferredColor.r);
    bb.emplaceFloat32(body.preferredColor.g);
    bb.emplaceFloat32(body.preferredColor.b);
    bb.emplaceFloat32(body.atmosphereRadius * scale);

    bb.emplaceFloat32((float)body.bodyId);
    bb.emplaceFloat32(body.terrainMaxLevel * scale);
    bb.emplaceFloat32(body.fluidMaxLevel * scale);
    bb.emplaceFloat32(body.habitableChance);

    bb.emplaceFloat32(body.atmosphereAbsorbColor.r);
    bb.emplaceFloat32(body.atmosphereAbsorbColor.g);
    bb.emplaceFloat32(body.atmosphereAbsorbColor.b);
    bb.emplaceFloat32(body.atmosphereAbsorbStrength);

    void* data;
    dataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    dataBuffer->unmap();
}

double RenderedCelestialBody::getDistance(glm::dvec3 position, double at_time)
{
    return glm::distance(position, body.getPosition(at_time));
}
