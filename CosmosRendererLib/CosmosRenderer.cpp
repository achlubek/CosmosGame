#include "stdafx.h"
#include "CosmosRenderer.h"
#include "SceneProvider.h"
#include "GalaxyContainer.h"
#include "AbsGameContainer.h"
#include "ModelsRenderer.h"
#include "TimeProvider.h"
#include "stdafx.h"
#include "vulkan.h"


CosmosRenderer::CosmosRenderer(VulkanToolkit* ivulkan, TimeProvider* itimeProvider, SceneProvider* isceneProvider, GalaxyContainer* igalaxy, VulkanImage* ioverlayImage, int iwidth, int iheight) :
    galaxy(igalaxy), overlayImage(ioverlayImage), width(iwidth), height(iheight), timeProvider(itimeProvider),
    vulkan(ivulkan), sceneProvider(isceneProvider), assets(AssetLoader(ivulkan)), renderablePlanets({}), renderableMoons({}), updatingSafetyQueue(InvokeQueue())
{
    internalCamera = new Camera();

    cube3dInfo = assets.loadObject3dInfoFile("cube1unitradius.raw");

    auto wholeIcoMesh = assets.loadObject3dInfoFile("icosphere_to_separate.raw");
    auto splitMesh = splitTriangles(wholeIcoMesh);
    for (int i = 0; i < splitMesh.size(); i++) {
        int g = 0;
        glm::vec3 v1 = glm::normalize(glm::vec3(splitMesh[i]->vbo[g], splitMesh[i]->vbo[g + 1], splitMesh[i]->vbo[g + 2]));
        g += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(splitMesh[i]->vbo[g], splitMesh[i]->vbo[g + 1], splitMesh[i]->vbo[g + 2]));
        g += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(splitMesh[i]->vbo[g], splitMesh[i]->vbo[g + 1], splitMesh[i]->vbo[g + 2]));

        glm::vec3 dir = glm::normalize((v1 + v2 + v3) / glm::vec3(3.0));
        auto low = subdivide(splitMesh[i]);
        auto medium = subdivide(subdivide(low));
        auto high = subdivide(subdivide(medium));
        patchesLowPoly.push_back({ dir, low });
        patchesMediumPoly.push_back({ dir, medium });
        patchesHighPoly.push_back({ dir, high });
    }


    icosphereLow = assets.loadObject3dInfoFile("icosphere_mediumpoly_1unit.raw");

    icosphereMedium = assets.loadObject3dInfoFile("icosphere_mediumpoly_1unit.raw");

    icosphereHigh = subdivide(icosphereMedium);// assets.loadObject3dInfoFile("icosphere_highpoly_1unit.raw");

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

    //#########//


    shadowmapsDepthMap = new VulkanImage(vulkan, shadowMapWidth, shadowMapHeight, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, true);

    shadowMapDataSetLayout = new VulkanDescriptorSetLayout(vulkan);
    shadowMapDataSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    shadowMapDataSetLayout->compile();

    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        shadowmaps.push_back(new VulkanImage(vulkan, shadowMapWidth, shadowMapHeight, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false));
        auto buff = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(float) * 1024);
        void* data;
        buff->map(0, sizeof(float) * 4, &data);
        float invdivisor = 1.0 / (float)shadowmapsDivisors[i];
        ((float*)(data))[0] = invdivisor;
        ((float*)(data))[1] = invdivisor;
        ((float*)(data))[2] = invdivisor;
        ((float*)(data))[3] = invdivisor;
        buff->unmap();

        shadowmapsBuffers.push_back(buff);

        auto dataSet = shadowMapDataSetLayout->generateDescriptorSet();
        dataSet->bindUniformBuffer(0, buff);
        dataSet->update();
        shadowmapsDataSets.push_back(dataSet);

    }


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



    combineLayout = new VulkanDescriptorSetLayout(vulkan);
    combineLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    combineLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(6, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->addField(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    combineLayout->compile();


    shadowMapsCollectionLayout = new VulkanDescriptorSetLayout(vulkan);
    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        shadowMapsCollectionLayout->addField(i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    }
    shadowMapsCollectionLayout->compile();

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

    celestialShadowMapSetLayout = new VulkanDescriptorSetLayout(vulkan);
    celestialShadowMapSetLayout->addField(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialShadowMapSetLayout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS);
    celestialShadowMapSetLayout->compile();


    combineSet = combineLayout->generateDescriptorSet();
    combineSet->bindUniformBuffer(0, cameraDataBuffer);
    combineSet->bindImageViewSampler(1, celestialAlphaImage);
    combineSet->bindImageViewSampler(2, starsImage);
    combineSet->bindImageViewSampler(3, overlayImage);
    combineSet->bindImageViewSampler(4, celestialAdditiveImage);
    combineSet->bindImageViewSampler(5, AbsGameContainer::getInstance()->getModelsRenderer()->getAlbedoRoughnessImage());
    combineSet->bindImageViewSampler(6, AbsGameContainer::getInstance()->getModelsRenderer()->getNormalMetalnessImage());
    combineSet->bindImageViewSampler(7, AbsGameContainer::getInstance()->getModelsRenderer()->getDistanceImage());
    combineSet->update();


    shadowMapsCollectionSet = shadowMapsCollectionLayout->generateDescriptorSet();
    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        shadowMapsCollectionSet->bindImageViewSampler(i, shadowmaps[i]);
    }
    shadowMapsCollectionSet->update();

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
        for (int i = 0; i < shadowmapsDivisors.size(); i++) {
            safedelete(celestialShadowMapRenderStages[i]);
        }
        celestialShadowMapRenderStages.clear();
    }
    vkDeviceWaitIdle(vulkan->device);


    //**********************//

    auto celestialshadowmapvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial-shadowmap.vert.spv");
    auto celestialshadowmapfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-celestial-shadowmap.frag.spv");

    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        auto celestialShadowMapRenderStage = new VulkanRenderStage(vulkan);
        celestialShadowMapRenderStage->setViewport(shadowMapWidth, shadowMapHeight);
        celestialShadowMapRenderStage->addShaderStage(celestialshadowmapvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
        celestialShadowMapRenderStage->addShaderStage(celestialshadowmapfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
        celestialShadowMapRenderStage->addDescriptorSetLayout(rendererDataLayout->layout);
        celestialShadowMapRenderStage->addDescriptorSetLayout(celestialShadowMapSetLayout->layout);
        celestialShadowMapRenderStage->addDescriptorSetLayout(shadowMapDataSetLayout->layout);
        celestialShadowMapRenderStage->addOutputImage(shadowmaps[i]);
        celestialShadowMapRenderStage->addOutputImage(shadowmapsDepthMap);
        celestialShadowMapRenderStage->cullFlags = 0;// VK_CULL_MODE_FRONT_BIT;
        celestialShadowMapRenderStage->compile();
        celestialShadowMapRenderStages.push_back(celestialShadowMapRenderStage);
    }

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
    celestialBodySurfaceRenderStage->cullFlags = VK_CULL_MODE_FRONT_BIT;
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
    celestialBodyWaterRenderStage->cullFlags = 0;// VK_CULL_MODE_FRONT_BIT;
    celestialBodyWaterRenderStage->compile();

    //**********************//
    auto celestialdatacompute = new VulkanShaderModule(vulkan, "../../shaders/compiled/celestial-updatedata.comp.spv");

    celestialDataUpdateComputeStage = new VulkanComputeStage(vulkan);
    celestialDataUpdateComputeStage->setShaderStage(celestialdatacompute->createShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, "main"));
    celestialDataUpdateComputeStage->addDescriptorSetLayout(celestialBodyDataSetLayout->layout);
    celestialDataUpdateComputeStage->queue = vulkan->secondaryQueue;
    celestialDataUpdateComputeStage->compile();

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
    celestialStage->addDescriptorSetLayout(shadowMapsCollectionLayout->layout);
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
    celestialAlphaImage->clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    starsStage->addOutputImage(starsImage);
    starsStage->setSets({ rendererDataSet, starsDataSet });
    starsStage->additiveBlending = true;
    starsStage->cullFlags = VK_CULL_MODE_BACK_BIT;
    // starsStage->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    starsStage->compile();
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
    double au1 = 14960000.0;
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


    bb.emplaceInt32(shadowmapsDivisors.size());
    bb.emplaceInt32(shadowmapsDivisors.size());
    bb.emplaceInt32(shadowmapsDivisors.size());
    bb.emplaceInt32(shadowmapsDivisors.size());

    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        bb.emplaceFloat32(shadowmapsDivisors[i]);
    }
    for (int i = 0; i < 3 - (shadowmapsDivisors.size() % 4); i++) {
        bb.emplaceFloat32(0.0f);
    }

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

    measureTimeStart();
    updatingSafetyQueue.executeAll();
    measureTimeEnd("Executing safety queue");

    measureTimeStart();
    if (!firstRecordingDone) {
        starsStage->beginDrawing();

        starsStage->drawMesh(cube3dInfo, galaxy->getStarsCount());

        starsStage->endDrawing();
    }
    starsStage->submit({});

    measureTimeEnd("Stars galaxy draw");

    //vkDeviceWaitIdle(vulkan->device);

    measureTimeStart();
    if (!firstRecordingDone) {
        celestialStarsBlitComputeStage->beginRecording();
        celestialStarsBlitComputeStage->dispatch({ celestiaStarsBlitSet }, width / 256 + 1, height / 2, 1);
        celestialStarsBlitComputeStage->endRecording();
    }
    firstRecordingDone = true;

    celestialStarsBlitComputeStage->submitNoSemaphores({ starsStage->signalSemaphore });

    measureTimeEnd("Stars blit");

    //vkDeviceWaitIdle(vulkan->device);

    measureTimeStart();

    auto renderables = std::vector<RenderedCelestialBody*>();
    for (int i = 0; i < renderablePlanets.size(); i++) {
        renderables.push_back(renderablePlanets[i]);
    }
    for (int i = 0; i < renderableMoons.size(); i++) {
        renderables.push_back(renderableMoons[i]);
    }
    if (renderables.size() > 0) {

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
        renderables[renderables.size() - 1]->resizeDataImages(4096, 4096, 1024, 1024);

    }

    measureTimeEnd("Preparing for celestial");

    measureTimeStart();
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
    measureTimeEnd("Celestial hi freq update");
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
        measureTimeStart();
        std::vector<Object3dInfo*> meshSequence = {};
        celestialBodySurfaceRenderStage->beginDrawing();

        celestialBodySurfaceRenderStage->setSets({ rendererDataSet, renderables[i]->renderSurfaceSet });

        double radius = renderables[i]->getRadius();
        auto position = renderables[i]->body.getPosition(timeProvider->getTime());
        glm::dquat rotmat = glm::inverse(glm::quat_cast(renderables[i]->body.getRotationMatrix(timeProvider->getTime())));
        double centerdist = glm::distance(position, observerCameraPosition);

        if (centerdist > radius * 4 || renderables[i]->getRenderMethod() == CelestialRenderMethod::thickAtmosphere) {
            meshSequence.push_back(icosphereLow);
            //celestialBodySurfaceRenderStage->drawMesh(icosphereLow, 1);
        } 
        else {
            for (int g = 0; g < patchesLowPoly.size(); g++) {
                glm::dvec3 position1 = (rotmat * glm::dvec3(std::get<0>(patchesLowPoly[g]))) * radius + position;
                double dist = glm::distance(observerCameraPosition, position1);
                if (dist < radius * 0.2) {
                    meshSequence.push_back(std::get<1>(patchesHighPoly[g]));
                  //  celestialBodySurfaceRenderStage->drawMesh(std::get<1>(patchesHighPoly[g]), 1);
                }
                else if (dist < radius * 3.6) {
                    meshSequence.push_back(std::get<1>(patchesMediumPoly[g]));
                  //  celestialBodySurfaceRenderStage->drawMesh(std::get<1>(patchesMediumPoly[g]), 1);
                }
                else {
                    meshSequence.push_back(std::get<1>(patchesLowPoly[g]));
                  //  celestialBodySurfaceRenderStage->drawMesh(std::get<1>(patchesLowPoly[g]), 1);
                }
            }
        }

        for (int g = 0; g < meshSequence.size(); g++) {
            celestialBodySurfaceRenderStage->drawMesh(meshSequence[g], 1);
        }

        //renderables[i]->drawSurface(celestialBodySurfaceRenderStage, rendererDataSet, i == (renderables.size() - 1) ? icosphereHigh : icosphereLow);

        celestialBodySurfaceRenderStage->endDrawing();
        celestialBodySurfaceRenderStage->submitNoSemaphores({  });

        measureTimeEnd("Celestial surface data for " + std::to_string(i));


        if (i == renderables.size() - 1) {
            for (int z = 0; z < shadowmapsDivisors.size(); z++) {
                measureTimeStart();
                celestialShadowMapRenderStages[z]->beginDrawing();
                celestialShadowMapRenderStages[z]->setSets({ rendererDataSet, renderables[i]->shadowMapSet, shadowmapsDataSets[z] });
                for (int g = 0; g < meshSequence.size(); g++) {
                    celestialShadowMapRenderStages[z]->drawMesh(meshSequence[g], 1);
                }
                celestialShadowMapRenderStages[z]->endDrawing();
                celestialShadowMapRenderStages[z]->submitNoSemaphores({});
                measureTimeEnd("Celestial shadow cascade "+std::to_string(z) +" data for " + std::to_string(i));
            }
        }


        if (renderables[i]->getRenderMethod() == CelestialRenderMethod::lightAtmosphere) {
            measureTimeStart();
            celestialBodyWaterRenderStage->beginDrawing();

            celestialBodyWaterRenderStage->setSets({ rendererDataSet, renderables[i]->renderWaterSet });

            if (centerdist < radius * 7.0) {
                celestialBodyWaterRenderStage->drawMesh(icosphereMedium, 1);
            }
            else {
                celestialBodyWaterRenderStage->drawMesh(icosphereLow, 1);
            }

            //renderables[i]->drawWater(celestialBodyWaterRenderStage, rendererDataSet, i == (renderables.size() - 1) ? icosphereMedium : icosphereLow);

            celestialBodyWaterRenderStage->endDrawing();
            celestialBodyWaterRenderStage->submitNoSemaphores({  });

            measureTimeEnd("Celestial water data for " + std::to_string(i));
        }

        measureTimeStart();

        celestialStage->beginDrawing();

        renderables[i]->draw(celestialStage, rendererDataSet, shadowMapsCollectionSet, cube3dInfo);

        celestialStage->endDrawing();
        celestialStage->submitNoSemaphores({ });
        measureTimeEnd("Celestial atmosphere and composite for " + std::to_string(i));
        vkDeviceWaitIdle(vulkan->device);
    }

    measureTimeStart();
    renderer->beginDrawing();

    renderer->endDrawing();
    measureTimeEnd("Composite output");

#ifdef PERFORMANCE_DEBUG
    printf("\n");
#endif
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

void CosmosRenderer::measureTimeStart()
{
#ifdef PERFORMANCE_DEBUG
    vkDeviceWaitIdle(vulkan->device);
    measurementStopwatch = glfwGetTime();
#endif
}

void CosmosRenderer::measureTimeEnd(std::string name)
{
#ifdef PERFORMANCE_DEBUG
    vkDeviceWaitIdle(vulkan->device);
    double end = glfwGetTime();
    printf("Time on [%s]: %f miliseconds\n", name.c_str(), 1000.0 * (end - measurementStopwatch));
#endif
}

Object3dInfo * CosmosRenderer::subdivide(Object3dInfo * info)
{
    std::vector<float> floats = {};
    for (int i = 0; i < info->vbo.size();) {
        glm::vec3 v1 = glm::normalize(glm::vec3(info->vbo[i], info->vbo[i + 1], info->vbo[i + 2]));
        i += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(info->vbo[i], info->vbo[i + 1], info->vbo[i + 2]));
        i += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(info->vbo[i], info->vbo[i + 1], info->vbo[i + 2]));
        i += 12;
        glm::vec3 tricenter = glm::normalize((v1 + v2 + v3) * glm::vec3(0.33333333));
        glm::vec3 v1tov2 = glm::normalize((v1 + v2) * glm::vec3(0.5));
        glm::vec3 v2tov3 = glm::normalize((v2 + v3) * glm::vec3(0.5));
        glm::vec3 v3tov1 = glm::normalize((v1 + v3) * glm::vec3(0.5));

        std::vector<glm::vec3> positions = {};
        positions.push_back(v3tov1);
        positions.push_back(v1);
        positions.push_back(v1tov2);

        positions.push_back(v1tov2);
        positions.push_back(v2);
        positions.push_back(v2tov3);

        positions.push_back(v2tov3);
        positions.push_back(v3);
        positions.push_back(v3tov1);

        positions.push_back(v1tov2);
        positions.push_back(v2tov3);
        positions.push_back(v3tov1);

        for (int g = 0; g < positions.size(); g++) {
            glm::vec3 v = positions[g];
            // px py pz ux uy nx ny nz tx ty tz tw | px
            // 0  1  2  3  4  5  6  7  8  9  10 11 | 12
            floats.push_back(v.x);
            floats.push_back(v.y);
            floats.push_back(v.z);

            floats.push_back(v.x);
            floats.push_back(v.y);

            floats.push_back(v.x);
            floats.push_back(v.y);
            floats.push_back(v.z);

            floats.push_back(v.x);
            floats.push_back(v.y);
            floats.push_back(v.z);
            floats.push_back(v.x);
        }
    }
    return new Object3dInfo(info->vulkan, floats);
}

std::vector<Object3dInfo*> CosmosRenderer::splitTriangles(Object3dInfo * info)
{
    std::vector<Object3dInfo*> objs = {};

    for (int i = 0; i < info->vbo.size();) {
        glm::vec3 v1 = glm::normalize(glm::vec3(info->vbo[i], info->vbo[i + 1], info->vbo[i + 2]));
        i += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(info->vbo[i], info->vbo[i + 1], info->vbo[i + 2]));
        i += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(info->vbo[i], info->vbo[i + 1], info->vbo[i + 2]));
        i += 12;

        auto buffer = std::vector<float>{
            v1.x,
            v1.y,
            v1.z,
            v1.x,
            v1.y,
            v1.x,
            v1.y,
            v1.z,
            v1.x,
            v1.y,
            v1.z,
            v1.x,

            v2.x,
            v2.y,
            v2.z,
            v2.x,
            v2.y,
            v2.x,
            v2.y,
            v2.z,
            v2.x,
            v2.y,
            v2.z,
            v2.x,

            v3.x,
            v3.y,
            v3.z,
            v3.x,
            v3.y,
            v3.x,
            v3.y,
            v3.z,
            v3.x,
            v3.y,
            v3.z,
            v3.x
        };
        objs.push_back(new Object3dInfo(info->vulkan, buffer));
    }
    return objs;
}
