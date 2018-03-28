#include "stdafx.h"
#include "CosmosRenderer.h"
#include "SceneProvider.h"
#include "GalaxyContainer.h"
#include "TimeProvider.h"
#include "stdafx.h"
#include "vulkan.h"
 

CosmosRenderer::CosmosRenderer(VulkanToolkit* ivulkan, TimeProvider* itimeProvider, SceneProvider* isceneProvider, GalaxyContainer* igalaxy, VulkanImage* ioverlayImage, int iwidth, int iheight) :
    galaxy(igalaxy), overlayImage(ioverlayImage), width(iwidth), height(iheight), timeProvider(itimeProvider),
    vulkan(ivulkan), sceneProvider(isceneProvider), assets(AssetLoader(ivulkan)), renderablePlanets({}), renderableMoons({}), updatingSafetyQueue(InvokeQueue())
{ 
    internalCamera = new Camera();

	cube3dInfo = assets.loadObject3dInfoFile("cube1unitradius.raw");

	icosphereLow = assets.loadObject3dInfoFile("icosphere_lowpoly_1unit.raw");

	icosphereMedium = assets.loadObject3dInfoFile("icosphere_mediumpoly_1unit.raw");

	icosphereHigh = assets.loadObject3dInfoFile("icosphere_highpoly_1unit.raw");

    cameraDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 1024);
    starsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 1024 * 1024 * 128); 
    planetsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    moonsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);

    celestialAlphaImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    celestialAdditiveImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    starsImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    modelsResultImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    modelsDepthImage = new VulkanImage(vulkan, width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

	//####################//

	surfaceRenderedAlbedoRoughnessImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

	surfaceRenderedNormalMetalnessImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

	surfaceRenderedDistanceImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

	surfaceRenderedDepthImage = new VulkanImage(vulkan, width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);


	//#######//

	waterRenderedNormalMetalnessImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

	waterRenderedDistanceImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

	waterRenderedDepthImage = new VulkanImage(vulkan, width, height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

	
	//####################//


    // cosmosImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
   //      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);

    rendererDataLayout = new VulkanDescriptorSetLayout(vulkan);
    rendererDataLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT);
    rendererDataLayout->compile();

    starsDataLayout = new VulkanDescriptorSetLayout(vulkan);
    starsDataLayout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    starsDataLayout->compile();

    rendererDataSet = rendererDataLayout->generateDescriptorSet();
    rendererDataSet->bindUniformBuffer(0, cameraDataBuffer);
    rendererDataSet->update();

    starsDataSet = starsDataLayout->generateDescriptorSet();
    starsDataSet->bindStorageBuffer(0, starsDataBuffer);
    starsDataSet->update();

    modelMRTLayout = new VulkanDescriptorSetLayout(vulkan);
    modelMRTLayout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    modelMRTLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->addField(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    modelMRTLayout->compile();


    combineLayout = new VulkanDescriptorSetLayout(vulkan);
    combineLayout->addField(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    combineLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->compile();

	celestialBodyDataSetLayout = new VulkanDescriptorSetLayout(vulkan);
	celestialBodyDataSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialBodyDataSetLayout->addField(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialBodyDataSetLayout->addField(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialBodyDataSetLayout->addField(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialBodyDataSetLayout->compile();

    celestiaStarsBlitSetLayout = new VulkanDescriptorSetLayout(vulkan);
    celestiaStarsBlitSetLayout->addField(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT);
    celestiaStarsBlitSetLayout->addField(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
    celestiaStarsBlitSetLayout->compile();

	celestialShadowMapSetLayout = new VulkanDescriptorSetLayout(vulkan);
	celestialShadowMapSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialShadowMapSetLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialShadowMapSetLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialShadowMapSetLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialShadowMapSetLayout->addField(4, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
	celestialShadowMapSetLayout->compile();

	celestialBodyRenderSetLayout = new VulkanDescriptorSetLayout(vulkan);
	celestialBodyRenderSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
	celestialBodyRenderSetLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->addField(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->addField(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->addField(8, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->addField(9, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	celestialBodyRenderSetLayout->compile();

	celestialBodySurfaceSetLayout = new VulkanDescriptorSetLayout(vulkan);
	celestialBodySurfaceSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
	celestialBodySurfaceSetLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS);
	celestialBodySurfaceSetLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS);
	celestialBodySurfaceSetLayout->compile();

	celestialBodyWaterSetLayout = new VulkanDescriptorSetLayout(vulkan);
	celestialBodyWaterSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
	celestialBodyWaterSetLayout->compile();

    combineSet = combineLayout->generateDescriptorSet();
    combineSet->bindImageViewSampler(0, celestialAlphaImage);
    combineSet->bindImageViewSampler(1, starsImage);
    combineSet->bindImageViewSampler(2, overlayImage);
    combineSet->bindUniformBuffer(3, cameraDataBuffer);
    combineSet->bindImageViewSampler(4, modelsResultImage);
    combineSet->bindImageViewSampler(5, celestialAdditiveImage);
    combineSet->update();

    celestiaStarsBlitSet = celestiaStarsBlitSetLayout->generateDescriptorSet();
    celestiaStarsBlitSet->bindImageViewSampler(0, starsImage);
    celestiaStarsBlitSet->bindImageStorage(1, celestialAlphaImage);
    celestiaStarsBlitSet->update();

    recompileShaders(false);

    galaxy->onClosestStarChange.add([&](GeneratedStarInfo star) { onClosestStarChange(star); });
    galaxy->onClosestPlanetChange.add([&](CelestialBody planet) { onClosestPlanetChange(planet); });
    
    readyForDrawing = true;
}


CosmosRenderer::~CosmosRenderer()
{
}

#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}
void CosmosRenderer::recompileShaders(bool deleteOld)
{
    readyForDrawing = false;
    if (deleteOld) {
        vkDeviceWaitIdle(vulkan->device);
		safedelete(celestialBodySurfaceRenderStage);
		safedelete(celestialBodyWaterRenderStage);
        safedelete(celestialStage);
        safedelete(starsStage);
        safedelete(combineStage);
        safedelete(renderer);
    }
    vkDeviceWaitIdle(vulkan->device);


	//**********************//

	auto celestialsurfacevert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial-surface.vert.spv");
	auto celestialsurfacefrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial-surface.frag.spv");

	celestialBodySurfaceRenderStage = new VulkanRenderStage(vulkan);
	celestialBodySurfaceRenderStage->setViewport(width, height);
	celestialBodySurfaceRenderStage->addShaderStage(celestialsurfacevert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
	celestialBodySurfaceRenderStage->addShaderStage(celestialsurfacefrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
	celestialBodySurfaceRenderStage->addDescriptorSetLayout(rendererDataLayout->layout);
	celestialBodySurfaceRenderStage->addDescriptorSetLayout(celestialBodySurfaceSetLayout->layout);
	celestialBodySurfaceRenderStage->addOutputImage(surfaceRenderedAlbedoRoughnessImage);
	celestialBodySurfaceRenderStage->addOutputImage(surfaceRenderedNormalMetalnessImage);
	celestialBodySurfaceRenderStage->addOutputImage(surfaceRenderedDistanceImage);
	celestialBodySurfaceRenderStage->addOutputImage(surfaceRenderedDepthImage);
	celestialBodySurfaceRenderStage->cullFlags = 0;
	celestialBodySurfaceRenderStage->compile();

	//**********************//

	auto celestialwatervert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial-water.vert.spv");
	auto celestialwaterfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial-water.frag.spv");

	celestialBodyWaterRenderStage = new VulkanRenderStage(vulkan);
	celestialBodyWaterRenderStage->setViewport(width, height);
	celestialBodyWaterRenderStage->addShaderStage(celestialwatervert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
	celestialBodyWaterRenderStage->addShaderStage(celestialwaterfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
	celestialBodyWaterRenderStage->addDescriptorSetLayout(rendererDataLayout->layout);
	celestialBodyWaterRenderStage->addDescriptorSetLayout(celestialBodyWaterSetLayout->layout);
	celestialBodyWaterRenderStage->addOutputImage(waterRenderedNormalMetalnessImage);
	celestialBodyWaterRenderStage->addOutputImage(waterRenderedDistanceImage);
	celestialBodyWaterRenderStage->addOutputImage(waterRenderedDepthImage);
	celestialBodyWaterRenderStage->cullFlags = 0;
	celestialBodyWaterRenderStage->compile();

	//**********************//
	auto celestialdatacompute = new VulkanShaderModule(vulkan, "../../shaders/compiled/celestial-updatedata.comp.spv");

	celestialDataUpdateComputeStage = new VulkanComputeStage(vulkan);
	celestialDataUpdateComputeStage->setShaderStage(celestialdatacompute->createShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, "main"));
	celestialDataUpdateComputeStage->addDescriptorSetLayout(celestialBodyDataSetLayout->layout);
	celestialDataUpdateComputeStage->queue = vulkan->secondaryQueue;
	celestialDataUpdateComputeStage->compile();

	//**********************//
	auto celestialshadowmapcompute = new VulkanShaderModule(vulkan, "../../shaders/compiled/celestial-updateshadows.comp.spv");

	celestialShadowMapComputeStage = new VulkanComputeStage(vulkan);
	celestialShadowMapComputeStage->setShaderStage(celestialshadowmapcompute->createShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, "main"));
	celestialShadowMapComputeStage->addDescriptorSetLayout(rendererDataLayout->layout);
	celestialShadowMapComputeStage->addDescriptorSetLayout(celestialShadowMapSetLayout->layout);
	celestialShadowMapComputeStage->queue = vulkan->secondaryQueue;
	celestialShadowMapComputeStage->compile();

    //**********************//
    auto celestialblitcompute = new VulkanShaderModule(vulkan, "../../shaders/compiled/celestial-blit-stars.comp.spv");

    celestialStarsBlitComputeStage = new VulkanComputeStage(vulkan);
    celestialStarsBlitComputeStage->setShaderStage(celestialblitcompute->createShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, "main"));
    celestialStarsBlitComputeStage->addDescriptorSetLayout(celestiaStarsBlitSetLayout->layout);
    celestialStarsBlitComputeStage->compile();

    //**********************//

    auto celestialvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial.vert.spv");
    auto celestialfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial.frag.spv");

    celestialStage = new VulkanRenderStage(vulkan);
    celestialStage->setViewport(width, height);
    celestialStage->addShaderStage(celestialvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    celestialStage->addShaderStage(celestialfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    celestialStage->addDescriptorSetLayout(rendererDataLayout->layout);
    celestialStage->addDescriptorSetLayout(celestialBodyRenderSetLayout->layout);
    celestialAlphaImage->attachmentBlending = VulkanAttachmentBlending::Alpha;
    celestialAlphaImage->clear = false;
    celestialAdditiveImage->attachmentBlending = VulkanAttachmentBlending::Additive;
    celestialAdditiveImage->clear = true;
    celestialStage->addOutputImage(celestialAlphaImage);
    celestialStage->addOutputImage(celestialAdditiveImage);
   // celestialStage->alphaBlending = true;
    celestialStage->cullFlags = VK_CULL_MODE_BACK_BIT;
    celestialStage->clearBeforeDrawing = true;
    celestialStage->compile();

    //**********************//
    auto starsvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-stars.vert.spv");
    auto starsfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-stars.frag.spv");

    starsStage = new VulkanRenderStage(vulkan);
    starsStage->setViewport(width, height);
    starsStage->addShaderStage(starsvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    starsStage->addShaderStage(starsfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    starsStage->addDescriptorSetLayout(rendererDataLayout->layout);
    starsStage->addDescriptorSetLayout(starsDataLayout->layout);
    //starsStage->addOutputImage(starsImage);
    celestialAlphaImage->clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    starsStage->addOutputImage(starsImage);
    starsStage->setSets({ rendererDataSet, starsDataSet });
    starsStage->additiveBlending = true;
    starsStage->cullFlags = VK_CULL_MODE_BACK_BIT;
    // starsStage->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    starsStage->compile();
    //**********************//

    auto shipvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-ship.vert.spv");
    auto shipfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-ship.frag.spv");

    modelsStage = new VulkanRenderStage(vulkan);
    modelsStage->setViewport(width, height);
    modelsStage->addShaderStage(shipvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    modelsStage->addShaderStage(shipfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    modelsStage->addDescriptorSetLayout(rendererDataLayout->layout);
    modelsStage->addDescriptorSetLayout(modelMRTLayout->layout);
    modelsStage->addOutputImage(modelsResultImage);
    modelsStage->addOutputImage(modelsDepthImage);
    modelsStage->cullFlags = 0; 
    modelsStage->compile();

    //**********************//

    auto combinevert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-combine.vert.spv");
    auto combinefrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-combine.frag.spv");

    combineStage = new VulkanRenderStage(vulkan);
    combineStage->setViewport(width, height);
    combineStage->addShaderStage(combinevert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    combineStage->addShaderStage(combinefrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    combineStage->addDescriptorSetLayout(combineLayout->layout);
    combineStage->setSets({ combineSet });
    //combineStage->addOutputImage(outputImage); //outputs to swapchain, managed automatically

    //**********************//

    renderer = new VulkanRenderer(vulkan);
    renderer->setOutputStage(combineStage);
    renderer->compile();

    if (deleteOld) {
        onClosestPlanetChange(galaxy->getClosestPlanet());
    }

    readyForDrawing = true;
    firstRecordingDone = false;
}

void CosmosRenderer::mapBuffers()
{
    starsDataBuffer->map(0, starsDataBuffer->size, &starsDataBufferPointer);
    planetsDataBuffer->map(0, planetsDataBuffer->size, &planetsDataBufferPointer);
    moonsDataBuffer->map(0, moonsDataBuffer->size, &moonsDataBufferPointer);
}

void CosmosRenderer::unmapBuffers()
{
    starsDataBuffer->unmap();
    planetsDataBuffer->unmap();
    moonsDataBuffer->unmap();
}

void CosmosRenderer::updateStarsBuffer()
{
    VulkanBinaryBufferBuilder starsBB = VulkanBinaryBufferBuilder();
    auto stars = galaxy->getAllStars();
    starsBB.emplaceInt32(stars.size());
    starsBB.emplaceInt32(stars.size());
    starsBB.emplaceInt32(stars.size());
    starsBB.emplaceInt32(stars.size());
    for (int s = 0; s < stars.size(); s++) {
        auto star = stars[s];

        glm::dvec3 starpos = star.getPosition(timeProvider->getTime()) * scale;

        starsBB.emplaceFloat32((float)starpos.x);
        starsBB.emplaceFloat32((float)starpos.y);
        starsBB.emplaceFloat32((float)starpos.z);
        starsBB.emplaceFloat32((float)star.radius * scale);

        starsBB.emplaceFloat32((float)star.color.x);
        starsBB.emplaceFloat32((float)star.color.y);
        starsBB.emplaceFloat32((float)star.color.z);
        starsBB.emplaceFloat32((float)star.age);

        starsBB.emplaceFloat32((float)star.orbitPlane.x);
        starsBB.emplaceFloat32((float)star.orbitPlane.y);
        starsBB.emplaceFloat32((float)star.orbitPlane.z);
        starsBB.emplaceFloat32((float)star.rotationSpeed);

        starsBB.emplaceFloat32((float)star.spotsIntensity);
        starsBB.emplaceFloat32((float)0.0f);
        starsBB.emplaceFloat32((float)0.0f);
        starsBB.emplaceFloat32((float)0.0f);
    }
    memcpy(starsDataBufferPointer, starsBB.getPointer(), starsBB.buffer.size());
}


void CosmosRenderer::updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition)
{
    internalCamera->projectionMatrix = camera->projectionMatrix;
    internalCamera->transformation->setOrientation(camera->transformation->getOrientation());
    internalCamera->transformation->setPosition(glm::vec3(0.0));
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    glfwGetCursorPos(vulkan->window, &xpos, &ypos);

    glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    glm::mat4 vpmatrix = clip * internalCamera->projectionMatrix * internalCamera->transformation->getInverseWorldTransform();

    glm::mat4 cameraViewMatrix = internalCamera->transformation->getInverseWorldTransform();
    glm::mat4 cameraRotMatrix = internalCamera->transformation->getRotationMatrix();
    glm::mat4 rpmatrix = internalCamera->projectionMatrix * inverse(cameraRotMatrix);
    internalCamera->cone->update(inverse(rpmatrix));

    auto star = galaxy->getClosestStar();
    glm::dvec3 closesStarRelPos = (star.getPosition(timeProvider->getTime()) - observerPosition) * scale;

    bb.emplaceFloat32((float)timeProvider->getTime());
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)xpos / (float)width);
    bb.emplaceFloat32((float)ypos / (float)height);
    bb.emplaceGeneric((unsigned char*)&rpmatrix, sizeof(rpmatrix));

    glm::vec3 newcamerapos = glm::vec3(observerPosition * scale);
    bb.emplaceGeneric((unsigned char*)&newcamerapos, sizeof(internalCamera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);

    bb.emplaceGeneric((unsigned char*)&(internalCamera->cone->leftBottom), sizeof(internalCamera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(internalCamera->cone->rightBottom - internalCamera->cone->leftBottom), sizeof(internalCamera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceGeneric((unsigned char*)&(internalCamera->cone->leftTop - internalCamera->cone->leftBottom), sizeof(internalCamera->cone->leftBottom));
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)width);
    bb.emplaceFloat32((float)height);
    bb.emplaceFloat32(exposure);
    bb.emplaceFloat32(0.0f);

    bb.emplaceFloat32(closesStarRelPos.x);
    bb.emplaceFloat32(closesStarRelPos.y);
    bb.emplaceFloat32(closesStarRelPos.z);
    bb.emplaceFloat32(0.0f);

	// calculate lux of closest star
	// assuming 1 AU = 1 490 000 
	// and lux at 1 AU = 120 000
	// the formula for AU coefficent = distance / AU1
	// the formula for lux coefficent = 1.0 / (AUcoefficent * AUcoefficent)
	// the formula for final lux is lux multiplier * 120 000
	double au1 = 1490000.0;
	double aucoeff = galaxy->getClosestPlanet().hostDistance / au1;
	double luxcoeff = 1.0 / (aucoeff * aucoeff);
	double lux = luxcoeff * 120000.0;

	/*
	lux = 120000 / (distance / 1 AU) ^ 2
	*/

    bb.emplaceFloat32(star.color.r * lux);
    bb.emplaceFloat32(star.color.g * lux);
    bb.emplaceFloat32(star.color.b * lux);
    bb.emplaceFloat32(0.0f);

    void* data;
    cameraDataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    cameraDataBuffer->unmap();

    observerCameraPosition = observerPosition;

}

void CosmosRenderer::draw()
{
    if (!readyForDrawing) return;

    //galaxy->update(observerCameraPosition);

    updatingSafetyQueue.executeAll();

    if (!firstRecordingDone) {
        starsStage->beginDrawing();

        starsStage->drawMesh(cube3dInfo, galaxy->getStarsCount());

        starsStage->endDrawing();
    }
    starsStage->submit({});

    //vkDeviceWaitIdle(vulkan->device);

    if (!firstRecordingDone) {
        celestialStarsBlitComputeStage->beginRecording();
        celestialStarsBlitComputeStage->dispatch({ celestiaStarsBlitSet }, width, height, 1);
        celestialStarsBlitComputeStage->endRecording();
    }
    firstRecordingDone = true;

    celestialStarsBlitComputeStage->submitNoSemaphores({ starsStage->signalSemaphore});
    
    //vkDeviceWaitIdle(vulkan->device);


    auto renderables = std::vector<RenderedCelestialBody*>();
    for (int i = 0; i < renderablePlanets.size(); i++) {
        renderables.push_back(renderablePlanets[i]);
    }
    for (int i = 0; i < renderableMoons.size(); i++) {
        renderables.push_back(renderableMoons[i]);
    }

	for (int a = 0; a < renderables.size(); a++) {
		for (int b = 0; b < renderables.size(); b++) {
			double dist_a = renderables[a]->getDistance(observerCameraPosition, timeProvider->getTime());
			double dist_b = renderables[b]->getDistance(observerCameraPosition, timeProvider->getTime());
			if (dist_a > dist_b) {
				auto tmp = renderables[b];
				renderables[b] = renderables[a];
				renderables[a] = tmp;
			}
		}
	}

	for (int a = 0; a < renderables.size() - 1; a++) {
		renderables[a]->resizeDataImages(256, 256, 256, 256);
	}
	renderables[renderables.size() - 1]->resizeDataImages(2048, 2048, 1024, 1024);

	celestialDataUpdateComputeStage->beginRecording();
	for (int a = 0; a < renderables.size(); a++) {
		if (renderables[a]->needsDataUpdate()) {
			renderables[a]->updateData(celestialDataUpdateComputeStage);
		}
	}
	celestialDataUpdateComputeStage->endRecording();
	celestialDataUpdateComputeStage->submitNoSemaphores({});

	for (int a = 0; a < renderables.size(); a++) {
		renderables[a]->updateBuffer(observerCameraPosition, scale, timeProvider->getTime());
	}
	/*
	if (renderables.size() > 0) {
		celestialShadowMapComputeStage->beginRecording();
		//for (int a = 0; a < renderables.size(); a++) {
		if (shadowMapRoundRobinCounter >= renderables.size() - 1) shadowMapRoundRobinCounter = 0;
		renderables[shadowMapRoundRobinCounter]->updateShadows(celestialShadowMapComputeStage, rendererDataSet);
		shadowMapRoundRobinCounter++;
		//}

		renderables[renderables.size() - 1]->updateShadows(celestialShadowMapComputeStage, rendererDataSet);
		celestialShadowMapComputeStage->endRecording();
		celestialShadowMapComputeStage->submitNoSemaphores({  });
	}*/

	for (int i = 0; i < renderables.size(); i++) {
		celestialBodySurfaceRenderStage->beginDrawing();

		renderables[i]->drawSurface(celestialBodySurfaceRenderStage, rendererDataSet, i == (renderables.size() - 1) ? icosphereHigh : icosphereLow);

		celestialBodySurfaceRenderStage->endDrawing();
		celestialBodySurfaceRenderStage->submitNoSemaphores({  });

		celestialBodyWaterRenderStage->beginDrawing();

		renderables[i]->drawWater(celestialBodyWaterRenderStage, rendererDataSet, i == (renderables.size() - 1) ? icosphereHigh : icosphereLow);

		celestialBodyWaterRenderStage->endDrawing();
		celestialBodyWaterRenderStage->submitNoSemaphores({  });

		celestialStage->beginDrawing();

        renderables[i]->draw(celestialStage, rendererDataSet, cube3dInfo);

		celestialStage->endDrawing();
		celestialStage->submitNoSemaphores({ });
		vkDeviceWaitIdle(vulkan->device);
    }


    vkDeviceWaitIdle(vulkan->device);
    modelsStage->beginDrawing();

    vkDeviceWaitIdle(vulkan->device);
    //for (int i = 0; i < ships.size(); i++)ships[i]->drawShipAndModules(modelsStage, celestialSet, observerCameraPosition);
    //GameContainer::getInstance()->drawDrawableObjects();
    sceneProvider->drawDrawableObjects(modelsStage, rendererDataSet);


    vkDeviceWaitIdle(vulkan->device);
    modelsStage->endDrawing();
    vkDeviceWaitIdle(vulkan->device);
    modelsStage->submitNoSemaphores({});
    vkDeviceWaitIdle(vulkan->device);

    renderer->beginDrawing();

    renderer->endDrawing();
}

void CosmosRenderer::onClosestStarChange(GeneratedStarInfo star)
{
}

void CosmosRenderer::onClosestPlanetChange(CelestialBody planet)
{

    updatingSafetyQueue.enqueue([&]() {

        for (int i = 0; i < renderablePlanets.size(); i++) {
            delete renderablePlanets[i];
            renderablePlanets[i] = nullptr;
        }
        renderablePlanets.clear();
        auto renderable = new RenderedCelestialBody(vulkan,
			galaxy->getClosestPlanet(), 
			celestialBodyDataSetLayout, 
			celestialShadowMapSetLayout, 
			celestialBodyRenderSetLayout,
			celestialBodySurfaceSetLayout, 
			celestialBodyWaterSetLayout,
			surfaceRenderedAlbedoRoughnessImage,
			surfaceRenderedNormalMetalnessImage,
			surfaceRenderedDistanceImage,
			waterRenderedNormalMetalnessImage,
			waterRenderedDistanceImage);
        renderable->updateBuffer(observerCameraPosition, scale, timeProvider->getTime());
        renderablePlanets.push_back(renderable);
        //renderable->updateData(celestialDataUpdateComputeStage);

        vkDeviceWaitIdle(vulkan->device);

        for (int i = 0; i < renderableMoons.size(); i++) {
            delete renderableMoons[i];
            renderableMoons[i] = nullptr;
        }
        renderableMoons.clear();
        auto moons = galaxy->getClosestPlanetMoons();
        for (int i = 0; i < moons.size(); i++) {
            auto renderable = new RenderedCelestialBody(vulkan, 
				moons[i], 
				celestialBodyDataSetLayout, 
				celestialShadowMapSetLayout, 
				celestialBodyRenderSetLayout,
				celestialBodySurfaceSetLayout, 
				celestialBodyWaterSetLayout,
				surfaceRenderedAlbedoRoughnessImage,
				surfaceRenderedNormalMetalnessImage,
				surfaceRenderedDistanceImage,
				waterRenderedNormalMetalnessImage,
				waterRenderedDistanceImage);
            renderable->updateBuffer(observerCameraPosition, scale, timeProvider->getTime());
            renderableMoons.push_back(renderable);
            //renderable->updateData(celestialDataUpdateComputeStage);
        }
      //  vkDeviceWaitIdle(vulkan->device);
     //   vkDeviceWaitIdle(vulkan->device);
    });//yay javascript
}
