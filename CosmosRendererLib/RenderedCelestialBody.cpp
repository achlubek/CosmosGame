#include "stdafx.h"
#include "CelestialBody.h"
#include "RenderedCelestialBody.h"


RenderedCelestialBody::RenderedCelestialBody(
    VulkanToolkit* toolkit,
    CelestialBody body,
    VulkanDescriptorSetLayout* dataSetLayout,
    VulkanDescriptorSetLayout* shadowMapSetLayout,
    VulkanDescriptorSetLayout* renderSetLayout,
    VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout,
    VulkanDescriptorSetLayout* celestialBodyWaterSetLayout,
    VulkanDescriptorSetLayout* celestialBodyRaycastUniqueSetLayout,
    VulkanImage* surfaceRenderedAlbedoRoughnessImage,
    VulkanImage* surfaceRenderedNormalMetalnessImage,
    VulkanImage* surfaceRenderedEmissionImage,
    VulkanImage* surfaceRenderedDistanceImage,
    VulkanImage* waterRenderedNormalMetalnessImage,
    VulkanImage* waterRenderedDistanceImage)
    : toolkit(toolkit), body(body),
    surfaceRenderedAlbedoRoughnessImage(surfaceRenderedAlbedoRoughnessImage),
    surfaceRenderedNormalMetalnessImage(surfaceRenderedNormalMetalnessImage),
    surfaceRenderedEmissionImage(surfaceRenderedEmissionImage),
    surfaceRenderedDistanceImage(surfaceRenderedDistanceImage),
    waterRenderedNormalMetalnessImage(waterRenderedNormalMetalnessImage),
    waterRenderedDistanceImage(waterRenderedDistanceImage)
{
    
    dataBuffer = toolkit->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, 65535);
    raycastResultsBuffer = toolkit->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, sizeof(float) * 1024 * 128);

    dataSet = dataSetLayout->generateDescriptorSet();
    
    shadowMapSet = shadowMapSetLayout->generateDescriptorSet();

    renderSet = renderSetLayout->generateDescriptorSet();

    renderSurfaceSet = celestialBodySurfaceSetLayout->generateDescriptorSet();

    renderWaterSet = celestialBodyWaterSetLayout->generateDescriptorSet();

    celestialBodyRaycastUniqueSet = celestialBodyRaycastUniqueSetLayout->generateDescriptorSet();
}

#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}

void RenderedCelestialBody::resizeDataImages(int ilowFreqWidth, int ilowFreqHeight, int ihiFreqWidth, int ihiFreqHeight)
{
    if (lowFreqWidth != ilowFreqWidth || lowFreqHeight != ilowFreqHeight) {
        lowFreqWidth = ilowFreqWidth;
        lowFreqHeight = ilowFreqHeight;

        safedelete(cloudsImage);
        safedelete(baseColorImage);
        safedelete(heightMapImage);

        heightMapImage = toolkit->getVulkanImageFactory()->build(lowFreqWidth, lowFreqHeight, VulkanImageFormat::R32f, VulkanImageUsage::Storage | VulkanImageUsage::Sampled);

        baseColorImage = toolkit->getVulkanImageFactory()->build(lowFreqWidth, lowFreqHeight, VulkanImageFormat::RGBA16f, VulkanImageUsage::Storage | VulkanImageUsage::Sampled);

        cloudsImage = toolkit->getVulkanImageFactory()->build(lowFreqWidth, lowFreqHeight, VulkanImageFormat::RG8unorm, VulkanImageUsage::Storage | VulkanImageUsage::Sampled);

        needsUpdate = true;
    }

    if (hiFreqWidth != ihiFreqWidth || hiFreqHeight != ihiFreqHeight) {
        hiFreqWidth = ihiFreqWidth;
        hiFreqHeight = ihiFreqHeight;

        safedelete(shadowMapImage);

        shadowMapImage = toolkit->getVulkanImageFactory()->build(hiFreqWidth, hiFreqHeight, VulkanImageFormat::RG16f, VulkanImageUsage::Storage | VulkanImageUsage::Sampled);

        shadowMapWidthOffset = 0;
    }    
    
    dataSet->bindBuffer(0, dataBuffer);
    dataSet->bindImageStorage(1, heightMapImage);
    dataSet->bindImageStorage(2, baseColorImage);
    dataSet->bindImageStorage(3, cloudsImage);

    celestialBodyRaycastUniqueSet->bindBuffer(0, dataBuffer);
    celestialBodyRaycastUniqueSet->bindImageViewSampler(1, heightMapImage);
    celestialBodyRaycastUniqueSet->bindImageViewSampler(2, baseColorImage);
    celestialBodyRaycastUniqueSet->bindImageViewSampler(3, cloudsImage);
    celestialBodyRaycastUniqueSet->bindBuffer(4, raycastResultsBuffer);
    

    shadowMapSet->bindBuffer(0, dataBuffer);
    shadowMapSet->bindImageViewSampler(1, heightMapImage);

    renderSet->bindBuffer(0, dataBuffer);
    renderSet->bindImageViewSampler(1, heightMapImage);
    renderSet->bindImageViewSampler(2, baseColorImage);
    renderSet->bindImageViewSampler(3, cloudsImage);
    renderSet->bindImageViewSampler(4, shadowMapImage);
    renderSet->bindImageViewSampler(5, surfaceRenderedAlbedoRoughnessImage);
    renderSet->bindImageViewSampler(6, surfaceRenderedNormalMetalnessImage);
    renderSet->bindImageViewSampler(7, surfaceRenderedEmissionImage);
    renderSet->bindImageViewSampler(8, surfaceRenderedDistanceImage);
    renderSet->bindImageViewSampler(9, waterRenderedNormalMetalnessImage);
    renderSet->bindImageViewSampler(10, waterRenderedDistanceImage);

    renderSurfaceSet->bindBuffer(0, dataBuffer);
    renderSurfaceSet->bindImageViewSampler(1, heightMapImage);
    renderSurfaceSet->bindImageViewSampler(2, baseColorImage);

    renderWaterSet->bindBuffer(0, dataBuffer);
    
    initialized = true;
}

RenderedCelestialBody::~RenderedCelestialBody()
{
    safedelete(renderSet);
    safedelete(dataSet);
    safedelete(dataBuffer);
    safedelete(raycastResultsBuffer);

    safedelete(shadowMapImage);
    safedelete(cloudsImage);
    safedelete(baseColorImage);
    safedelete(heightMapImage);
    safedelete(celestialBodyRaycastUniqueSet);
}

void RenderedCelestialBody::updateRaycasts(uint32_t raycastPointsCount, VulkanDescriptorSet * celestialBodyRaycastSharedSet, VulkanComputeStage * stage)
{
    if (!initialized) {
        return;
    }
    stage->dispatch({ celestialBodyRaycastSharedSet, celestialBodyRaycastUniqueSet }, raycastPointsCount, 1, 1);
}

bool RenderedCelestialBody::needsDataUpdate()
{
    return needsUpdate;
}

CelestialRenderMethod RenderedCelestialBody::getRenderMethod()
{
    return body.getRenderMethod();
}

double RenderedCelestialBody::getRadius()
{
    return body.radius;
}

std::vector<glm::dvec4> RenderedCelestialBody::getRaycastResults(int32_t count)
{
    std::vector<glm::dvec4> points = {};
    void* data;
    raycastResultsBuffer->map(0, raycastResultsBuffer->getSize(), &data);
    for (int i = 0; i < count; i++) {
        points.push_back(glm::dvec4(
            ((float*)data)[i * 4],
            ((float*)data)[i * 4 + 1],
            ((float*)data)[i * 4 + 2],
            ((float*)data)[i * 4 + 3]
        ));
    }
    raycastResultsBuffer->unmap();
    return points;
}

void RenderedCelestialBody::updateData(VulkanComputeStage * stage)
{
    if (!initialized) {
        return;
    }
    stage->dispatch({ dataSet }, lowFreqWidth / 256, lowFreqHeight / 2, 1);
    needsUpdate = false;
}

void RenderedCelestialBody::draw(VulkanRenderStage * stage, VulkanDescriptorSet* rendererDataSet, VulkanDescriptorSet* shadowMapsCollectionSet, Object3dInfo * info3d)
{
    if (!initialized) {
        return;
    }
    stage->setSets({ rendererDataSet, renderSet, shadowMapsCollectionSet });
    stage->drawMesh(info3d, 1);
}

void RenderedCelestialBody::drawSurface(VulkanRenderStage * stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo * info3d)
{
    if (!initialized) {
        return;
    }
    stage->setSets({ rendererDataSet, renderSurfaceSet });
    stage->drawMesh(info3d, 1);
}

void RenderedCelestialBody::drawWater(VulkanRenderStage * stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo * info3d)
{
    if (!initialized) {
        return;
    }
    stage->setSets({ rendererDataSet, renderWaterSet });
    stage->drawMesh(info3d, 1);
}

void RenderedCelestialBody::updateBuffer(glm::dvec3 observerPosition, double scale, double time)
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    bb.emplaceFloat32((float)time);
    bb.emplaceFloat32((float)lowFreqWidth);
    bb.emplaceFloat32((float)lowFreqHeight);
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)hiFreqWidth);
    bb.emplaceFloat32((float)hiFreqHeight);
    bb.emplaceFloat32((float)shadowMapWidthOffset);
    bb.emplaceFloat32(0.0f);

    auto bodyPosition = body.getPosition(time) - observerPosition;

    bb.emplaceInt32((int)body.getRenderMethod());
    bb.emplaceInt32(0);
    bb.emplaceInt32(0);
    bb.emplaceInt32(0);

    bb.emplaceFloat32((float)(bodyPosition.x * scale));
    bb.emplaceFloat32((float)(bodyPosition.y * scale));
    bb.emplaceFloat32((float)(bodyPosition.z * scale));
    bb.emplaceFloat32((float)(body.radius * scale));

    bb.emplaceFloat32(body.preferredColor.r);
    bb.emplaceFloat32(body.preferredColor.g);
    bb.emplaceFloat32(body.preferredColor.b);
    bb.emplaceFloat32((float)(body.atmosphereRadius * scale));

    bb.emplaceFloat32((float)body.bodyId);
    bb.emplaceFloat32((float)(body.terrainMaxLevel * scale));
    bb.emplaceFloat32((float)(body.fluidMaxLevel * scale));
    bb.emplaceFloat32((float)body.habitableChance);

    bb.emplaceFloat32(body.atmosphereAbsorbColor.r);
    bb.emplaceFloat32(body.atmosphereAbsorbColor.g);
    bb.emplaceFloat32(body.atmosphereAbsorbColor.b);
    bb.emplaceFloat32((float)body.atmosphereAbsorbStrength);

    auto rotmat = body.getRotationMatrix(time);
    bb.emplaceGeneric((unsigned char*)&rotmat, sizeof(rotmat));

    auto hostrotmat = body.getFromParentLookAtThisMatrix(time);
    bb.emplaceGeneric((unsigned char*)&hostrotmat, sizeof(hostrotmat));

    void* data;
    dataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    dataBuffer->unmap();
}

double RenderedCelestialBody::getDistance(glm::dvec3 position, double at_time)
{
    return glm::distance(position, body.getPosition(at_time));
}
