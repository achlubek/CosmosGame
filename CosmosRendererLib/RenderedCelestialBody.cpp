#include "stdafx.h"
#include "CelestialBody.h"
#include "RenderedCelestialBody.h"


RenderedCelestialBody::RenderedCelestialBody(
	VulkanToolkit* itoolkit,
	CelestialBody ibody,
	VulkanDescriptorSetLayout* dataSetLayout,
	VulkanDescriptorSetLayout* shadowMapSetLayout,
	VulkanDescriptorSetLayout* renderSetLayout,
	VulkanDescriptorSetLayout* celestialBodySurfaceSetLayout,
	VulkanDescriptorSetLayout* celestialBodyWaterSetLayout,
	VulkanImage* isurfaceRenderedAlbedoRoughnessImage,
	VulkanImage* isurfaceRenderedNormalMetalnessImage,
	VulkanImage* isurfaceRenderedDistanceImage,
	VulkanImage* iwaterRenderedNormalMetalnessImage,
	VulkanImage* iwaterRenderedDistanceImage)
	: toolkit(itoolkit), body(ibody),
	surfaceRenderedAlbedoRoughnessImage(isurfaceRenderedAlbedoRoughnessImage),
	surfaceRenderedNormalMetalnessImage(isurfaceRenderedNormalMetalnessImage),
	surfaceRenderedDistanceImage(isurfaceRenderedDistanceImage),
	waterRenderedNormalMetalnessImage(iwaterRenderedNormalMetalnessImage),
	waterRenderedDistanceImage(iwaterRenderedDistanceImage)
{

	dataBuffer = new VulkanGenericBuffer(toolkit, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 65535);

	dataSet = dataSetLayout->generateDescriptorSet();
	
	shadowMapSet = shadowMapSetLayout->generateDescriptorSet();

	renderSet = renderSetLayout->generateDescriptorSet();

	renderSurfaceSet = celestialBodySurfaceSetLayout->generateDescriptorSet();

	renderWaterSet = celestialBodyWaterSetLayout->generateDescriptorSet();
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

		heightMapImage = new VulkanImage(toolkit, lowFreqWidth, lowFreqHeight, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

		baseColorImage = new VulkanImage(toolkit, lowFreqWidth, lowFreqHeight, VK_FORMAT_R16G16B16A16_UNORM, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

		cloudsImage = new VulkanImage(toolkit, lowFreqWidth, lowFreqHeight, VK_FORMAT_R8G8_UNORM, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

		needsUpdate = true;
	}

	if (hiFreqWidth != ihiFreqWidth || hiFreqHeight != ihiFreqHeight) {
		hiFreqWidth = ihiFreqWidth;
		hiFreqHeight = ihiFreqHeight;

		safedelete(shadowMapImage);

		shadowMapImage = new VulkanImage(toolkit, hiFreqWidth, hiFreqHeight, VK_FORMAT_R16G16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

		shadowMapWidthOffset = 0;
	}	
	
	dataSet->bindUniformBuffer(0, dataBuffer);
	dataSet->bindImageStorage(1, heightMapImage);
	dataSet->bindImageStorage(2, baseColorImage);
	dataSet->bindImageStorage(3, cloudsImage);
	dataSet->update();

	shadowMapSet->bindUniformBuffer(0, dataBuffer);
	shadowMapSet->bindImageViewSampler(1, heightMapImage);
	shadowMapSet->bindImageViewSampler(2, baseColorImage);
	shadowMapSet->bindImageViewSampler(3, cloudsImage);
	shadowMapSet->bindImageStorage(4, shadowMapImage);
	shadowMapSet->update();

	renderSet->bindUniformBuffer(0, dataBuffer);
	renderSet->bindImageViewSampler(1, heightMapImage);
	renderSet->bindImageViewSampler(2, baseColorImage);
	renderSet->bindImageViewSampler(3, cloudsImage);
	renderSet->bindImageViewSampler(4, shadowMapImage);
	renderSet->bindImageViewSampler(5, surfaceRenderedAlbedoRoughnessImage);
	renderSet->bindImageViewSampler(6, surfaceRenderedNormalMetalnessImage);
	renderSet->bindImageViewSampler(7, surfaceRenderedDistanceImage);
	renderSet->bindImageViewSampler(8, waterRenderedNormalMetalnessImage);
	renderSet->bindImageViewSampler(9, waterRenderedDistanceImage);
	renderSet->update();

	renderSurfaceSet->bindUniformBuffer(0, dataBuffer);
	renderSurfaceSet->bindImageViewSampler(1, heightMapImage);
	renderSurfaceSet->bindImageViewSampler(2, baseColorImage);
	renderSurfaceSet->update();

	renderWaterSet->bindUniformBuffer(0, dataBuffer);
	renderWaterSet->update();
	
	initialized = true;
}

RenderedCelestialBody::~RenderedCelestialBody()
{
	safedelete(renderSet);
	safedelete(dataSet);
	safedelete(dataBuffer);

	safedelete(shadowMapImage);
	safedelete(cloudsImage);
	safedelete(baseColorImage);
	safedelete(heightMapImage);
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

void RenderedCelestialBody::updateData(VulkanComputeStage * stage)
{
	if (!initialized) {
		return;
	}
	stage->dispatch({ dataSet }, lowFreqWidth / 256, lowFreqHeight / 2, 1);
	needsUpdate = false;
}

void RenderedCelestialBody::updateShadows(VulkanComputeStage * stage, VulkanDescriptorSet* rendererDataSet)
{
	if (!initialized) {
		return;
	}
	int offsetAdvance = 4;
	stage->dispatch({ rendererDataSet, shadowMapSet }, 1, hiFreqHeight / 32, 1);
	shadowMapWidthOffset += offsetAdvance;
	if (shadowMapWidthOffset == hiFreqWidth) shadowMapWidthOffset = 0;
}

void RenderedCelestialBody::draw(VulkanRenderStage * stage, VulkanDescriptorSet* rendererDataSet, Object3dInfo * info3d)
{
	if (!initialized) {
		return;
	}
	stage->setSets({ rendererDataSet, renderSet });
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
