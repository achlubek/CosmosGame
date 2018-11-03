#include "stdafx.h"
#include "CosmosRenderer.h"


CosmosRenderer::CosmosRenderer(VulkanToolkit* vulkan, GalaxyContainer* galaxy, int width, int height) :
    galaxy(galaxy), width(width), height(height),
    vulkan(vulkan), renderablePlanets({}), renderableMoons({}), internalCommandBus(new CommandBus()),
    subdividedMeshesProvider(new SubdividedMeshesProvider(vulkan))
{
    //  internalCamera = new Camera();

    cube3dInfo = vulkan->getObject3dInfoFactory()->build("cube1unitradius.raw");


    outputImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    uiOutputImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    outputScreenRenderer = new OutputScreenRenderer(vulkan, width, height, outputImage, uiOutputImage);

    cameraDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 1024);
    raycastRequestsDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, sizeof(float) * 1024 * 128);
    
    celestialAlphaImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Storage | VulkanImageUsage::Sampled);

    celestialAdditiveImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    //####################//

    surfaceRenderedAlbedoRoughnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA8unorm, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    surfaceRenderedEmissionImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA8unorm, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    surfaceRenderedNormalMetalnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    surfaceRenderedDistanceImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    renderedDepthImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth);

    //#######//

    waterRenderedNormalMetalnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    waterRenderedDistanceImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    //#########//

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

    modelsDataSet = modelsDataLayout->generateDescriptorSet();
    modelsDataSet->bindBuffer(0, cameraDataBuffer);

    //#########//

    shadowmapsDepthMap = vulkan->getVulkanImageFactory()->build(shadowMapWidth, shadowMapHeight, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth);

    shadowMapDataSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    shadowMapDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);

    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        shadowmaps.push_back(vulkan->getVulkanImageFactory()->build(shadowMapWidth, shadowMapHeight, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled));
        auto buff = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 1024);
        void* data;
        buff->map(0, sizeof(float) * 4, &data);
        float divisor = (float)shadowmapsDivisors[i];
        ((float*)(data))[0] = divisor;
        ((float*)(data))[1] = divisor;
        ((float*)(data))[2] = divisor;
        ((float*)(data))[3] = divisor;
        buff->unmap();

        shadowmapsBuffers.push_back(buff);

        auto dataSet = shadowMapDataSetLayout->generateDescriptorSet();
        dataSet->bindBuffer(0, buff);
        shadowmapsDataSets.push_back(dataSet);

    }

    //####################//

    rendererDataLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    rendererDataLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);

    rendererDataSet = rendererDataLayout->generateDescriptorSet();
    rendererDataSet->bindBuffer(0, cameraDataBuffer);


    shadowMapsCollectionLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        shadowMapsCollectionLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    }

    celestialBodyRaycastSharedSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodyRaycastSharedSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyRaycastSharedSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);

    celestialBodyRaycastSharedSet = celestialBodyRaycastSharedSetLayout->generateDescriptorSet();
    celestialBodyRaycastSharedSet->bindBuffer(0, cameraDataBuffer);
    celestialBodyRaycastSharedSet->bindBuffer(1, raycastRequestsDataBuffer);

    celestialBodyRaycastUniqueSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodyRaycastUniqueSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyRaycastUniqueSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyRaycastUniqueSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyRaycastUniqueSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyRaycastUniqueSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);

    celestialBodyDataSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);

    celestialStarsBlitSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialStarsBlitSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialStarsBlitSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);

    celestialBodyRenderSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    celestialBodyRenderSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    celestialBodySurfaceSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodySurfaceSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    celestialBodySurfaceSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    celestialBodySurfaceSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);

    celestialBodyWaterSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodyWaterSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);

    celestialShadowMapSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialShadowMapSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    celestialShadowMapSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);


    starsRenderer = new StarsRenderer(vulkan, width, height, scale, rendererDataLayout, rendererDataSet, galaxy);


    combineLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    combineLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    combineLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    combineLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    combineLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    combineSet = combineLayout->generateDescriptorSet();
    combineSet->bindBuffer(0, cameraDataBuffer);
    combineSet->bindImageViewSampler(1, celestialAlphaImage);
    combineSet->bindImageViewSampler(2, starsRenderer->getStarsImage());
    combineSet->bindImageViewSampler(3, celestialAdditiveImage);


    shadowMapsCollectionSet = shadowMapsCollectionLayout->generateDescriptorSet();
    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        shadowMapsCollectionSet->bindImageViewSampler(i, shadowmaps[i]);
    }

    celestiaStarsBlitSet = celestialStarsBlitSetLayout->generateDescriptorSet();
    celestiaStarsBlitSet->bindImageViewSampler(0, starsRenderer->getStarsImage());
    celestiaStarsBlitSet->bindImageStorage(1, celestialAlphaImage);

    recompileShaders(false);

    galaxy->onClosestStarChange.add([&](Star star) { onClosestStarChange(star); });
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
        vulkan->waitDeviceIdle();
        safedelete(celestialBodySurfaceRenderStage);
        safedelete(celestialBodyWaterRenderStage);
        safedelete(celestialStage);
        safedelete(combineStage);
        for (int i = 0; i < shadowmapsDivisors.size(); i++) {
            safedelete(celestialShadowMapRenderStages[i]);
        }
        celestialShadowMapRenderStages.clear();
    }
    vulkan->waitDeviceIdle();


    //**********************//

    auto celestialshadowmapvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-celestial-shadowmap.vert.spv");
    auto celestialshadowmapfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-celestial-shadowmap.frag.spv");
    auto shipvertshadow = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-ship-shadow.vert.spv");
    auto shipfragshadow = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-ship-shadow.frag.spv");

    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        auto celestialShadowMapRenderStage = vulkan->getVulkanRenderStageFactory()->build(shadowMapWidth, shadowMapHeight,
            { celestialshadowmapvert, celestialshadowmapfrag }, { rendererDataLayout, celestialShadowMapSetLayout, shadowMapDataSetLayout },
            {
                shadowmaps[i]->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
                shadowmapsDepthMap->getAttachment(VulkanAttachmentBlending::None)
            });
        celestialShadowMapRenderStages.push_back(celestialShadowMapRenderStage);

        auto shadowModelsStage = vulkan->getVulkanRenderStageFactory()->build(shadowMapWidth, shadowMapHeight, { shipvertshadow, shipfragshadow }, { modelsDataLayout, modelMRTLayout, shadowMapDataSetLayout }, {
            shadowmaps[i]->getAttachment(VulkanAttachmentBlending::None, false,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            shadowmapsDepthMap->getAttachment(VulkanAttachmentBlending::None, false)
            });
        modelsShadowMapRenderStages.push_back(shadowModelsStage);
    }

    //**********************//

    auto celestialsurfacevert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-celestial-surface.vert.spv");
    auto celestialsurfacefrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-celestial-surface.frag.spv");

    celestialBodySurfaceRenderStage = vulkan->getVulkanRenderStageFactory()->build(width, height,
        { celestialsurfacevert, celestialsurfacefrag }, { rendererDataLayout, celestialBodySurfaceSetLayout },
        {
            surfaceRenderedAlbedoRoughnessImage->getAttachment(VulkanAttachmentBlending::None, false,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            surfaceRenderedEmissionImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
            surfaceRenderedNormalMetalnessImage->getAttachment(VulkanAttachmentBlending::None, false,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            surfaceRenderedDistanceImage->getAttachment(VulkanAttachmentBlending::None, false,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            renderedDepthImage->getAttachment(VulkanAttachmentBlending::None, false)
        }, VulkanCullMode::CullModeFront);

    //**********************//

    auto celestialwatervert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-celestial-water.vert.spv");
    auto celestialwaterfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-celestial-water.frag.spv");

    celestialBodyWaterRenderStage = vulkan->getVulkanRenderStageFactory()->build(width, height,
        { celestialwatervert, celestialwaterfrag }, { rendererDataLayout, celestialBodyWaterSetLayout },
        {
            waterRenderedNormalMetalnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            waterRenderedDistanceImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } })
          //  renderedDepthImage->getAttachment(VulkanAttachmentBlending::None)
        }, VulkanCullMode::CullModeFront);

    //**********************//

    auto shipvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-ship.vert.spv");
    auto shipfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-ship.frag.spv");

    modelsStage = vulkan->getVulkanRenderStageFactory()->build(width, height, { shipvert, shipfrag }, { modelsDataLayout, modelMRTLayout }, {
        surfaceRenderedAlbedoRoughnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        surfaceRenderedEmissionImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        surfaceRenderedNormalMetalnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        surfaceRenderedDistanceImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } }),
        renderedDepthImage->getAttachment(VulkanAttachmentBlending::None)
        });

    //**********************//




    

    //**********************//
    auto celestialdatacompute = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "celestial-updatedata.comp.spv");

    celestialDataUpdateComputeStage = vulkan->getVulkanComputeStageFactory()->build(celestialdatacompute, { celestialBodyDataSetLayout });

    //**********************//
    auto celestialdataraycast = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "celestial-raycast.comp.spv");

    celestialBodyRaycastComputeStage = vulkan->getVulkanComputeStageFactory()->build(celestialdataraycast, { celestialBodyRaycastSharedSetLayout, celestialBodyRaycastUniqueSetLayout });

    //**********************//
    auto celestialblitcompute = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "celestial-blit-stars.comp.spv");

    celestialStarsBlitComputeStage = vulkan->getVulkanComputeStageFactory()->build(celestialblitcompute, { celestialStarsBlitSetLayout });

    //**********************//

    auto celestialvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-celestial.vert.spv");
    auto celestialfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-celestial.frag.spv");

    celestialStage = vulkan->getVulkanRenderStageFactory()->build(width, height,
        { celestialvert, celestialfrag }, { rendererDataLayout, celestialBodyRenderSetLayout, shadowMapsCollectionLayout },
        {
            celestialAlphaImage->getAttachment(VulkanAttachmentBlending::Alpha, false),
            celestialAdditiveImage->getAttachment(VulkanAttachmentBlending::Additive, true, { { 0.0f, 0.0f, 0.0f, 0.0f } })
        }, VulkanCullMode::CullModeBack);

    //**********************//

    auto combinevert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-combine.vert.spv");
    auto combinefrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-combine.frag.spv");

    combineStage = vulkan->getVulkanRenderStageFactory()->build(width, height,
        { combinevert, combinefrag }, { combineLayout },
        {
            outputImage->getAttachment(VulkanAttachmentBlending::None, true),
        });
    combineStage->setSets({ combineSet });

    //**********************//

    if (deleteOld) {
        onClosestPlanetChange(galaxy->getClosestPlanet());
    }

    readyForDrawing = true;
    firstRecordingDone = false;
}

void CosmosRenderer::updateCameraBuffer(Camera * camera, double time)
{
    observerCameraPosition = camera->getPosition();
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    auto cursorpos = vulkan->getMouse()->getCursorPosition();
    xpos = std::get<0>(cursorpos);
    ypos = std::get<1>(cursorpos);

    glm::mat4 rpmatrix = camera->getRotationProjectionMatrix();
    auto cone = camera->getFrustumCone();

    auto star = galaxy->getClosestStar();
    
    auto fromStarToCameraMatrix = star.getFromThisLookAtPointMatrix(time, observerCameraPosition);
    //GameContainer::getInstance()->setCurrentSunDirection(fromStarToCameraMatrix);

    glm::dvec3 closesStarRelPos = (star.getPosition(time) - observerCameraPosition) * scale;

    bb.emplaceFloat32((float)time);
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)xpos / (float)width);
    bb.emplaceFloat32((float)ypos / (float)height);
    bb.emplaceGeneric((unsigned char*)&rpmatrix, sizeof(rpmatrix));

    glm::vec3 newcamerapos = glm::vec3(observerCameraPosition * scale);
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
    bb.emplaceFloat32(exposure);
    bb.emplaceFloat32(0.0f);

    bb.emplaceGeneric((unsigned char*)&fromStarToCameraMatrix, sizeof(fromStarToCameraMatrix));

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
    double au1 = 1496000.0;
    double aucoeff = (glm::distance(observerCameraPosition, galaxy->getClosestStar().getPosition(0))) / au1;
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


}

void CosmosRenderer::draw(SceneProvider* scene, double time)
{
    if (!readyForDrawing) return;

    modelsStage->beginDrawing();
    
    scene->drawDrawableObjects(modelsStage, modelsDataSet, scale);

    modelsStage->endDrawing();
    modelsStage->submit({});

#ifdef PERFORMANCE_DEBUG
    printf("{");
#endif
    //galaxy->update(observerCameraPosition);

    measureTimeStart();
    internalCommandBus->processQueue();
    measureTimeEnd("Executing safety queue");

    measureTimeStart();

    starsRenderer->draw({ modelsStage->getSignalSemaphore() });

    measureTimeEnd("Stars galaxy draw");

    //vkDeviceWaitIdle(vulkan->device);

    measureTimeStart();
    if (!firstRecordingDone) {
        celestialStarsBlitComputeStage->beginRecording();
        celestialStarsBlitComputeStage->dispatch({ celestiaStarsBlitSet }, width / 32 + 1, height / 2, 1);
        celestialStarsBlitComputeStage->endRecording();
    }
    firstRecordingDone = true;

    celestialStarsBlitComputeStage->submitNoSemaphores({ starsRenderer->getSignalSemaphore() });

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
                double dist_a = renderables[a]->getDistance(observerCameraPosition, time);
                double dist_b = renderables[b]->getDistance(observerCameraPosition, time);
                if (dist_a > dist_b) {
                    auto tmp = renderables[b];
                    renderables[b] = renderables[a];
                    renderables[a] = tmp;
                }
            }
        }

        for (int a = 0; a < renderables.size() - 1; a++) {
            renderables[a]->resizeDataImages(128, 128, 128, 128);
        }
        renderables[renderables.size() - 1]->resizeDataImages(1024, 1024, 1024, 1024);

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

    if (raycastPoints.size() > 0) {
        celestialBodyRaycastComputeStage->beginRecording();
        for (int a = 0; a < renderables.size(); a++) {
            renderables[a]->updateRaycasts(static_cast<uint32_t>(raycastPoints.size()), celestialBodyRaycastSharedSet, celestialBodyRaycastComputeStage);
        }
        celestialBodyRaycastComputeStage->endRecording();
        celestialBodyRaycastComputeStage->submitNoSemaphores({});
    }

    for (int a = 0; a < renderables.size(); a++) {
        renderables[a]->updateBuffer(observerCameraPosition, scale, time);
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
        auto position = renderables[i]->body.getPosition(time);
        glm::dquat rotmat = glm::inverse(glm::quat_cast(renderables[i]->body.getRotationMatrix(time)));
        double centerdist = glm::distance(position, observerCameraPosition);

        if (centerdist > radius * 3.0 || renderables[i]->getRenderMethod() == CelestialRenderMethod::thickAtmosphere) {
            meshSequence.push_back(subdividedMeshesProvider->getIcosphere(SubdividedMeshQuality::Low));
            //celestialBodySurfaceRenderStage->drawMesh(icosphereLow, 1);
        }
        else {
            meshSequence.push_back(subdividedMeshesProvider->getIcosphere(SubdividedMeshQuality::High)); // Pretty high performance hit here
            /*
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
            }*/
        }

        for (int g = 0; g < meshSequence.size(); g++) {
            celestialBodySurfaceRenderStage->drawMesh(meshSequence[g], 1);
        }

        //renderables[i]->drawSurface(celestialBodySurfaceRenderStage, rendererDataSet, i == (renderables.size() - 1) ? icosphereHigh : icosphereLow);

        celestialBodySurfaceRenderStage->endDrawing();
        celestialBodySurfaceRenderStage->submitNoSemaphores({  });

        measureTimeEnd("Celestial surface data for " + std::to_string(i));

        /*
        if (i == renderables.size() - 1) {
            for (int z = 0; z < shadowmapsDivisors.size(); z++) {
                measureTimeStart();
            //    int z = cascadeCounter;
           //     cascadeCounter++;
            //    if (cascadeCounter >= shadowmapsDivisors.size()) {
            //        cascadeCounter = 0;
            //    }
                celestialShadowMapRenderStages[z]->beginDrawing();
                celestialShadowMapRenderStages[z]->setSets({ rendererDataSet, renderables[i]->shadowMapSet, shadowmapsDataSets[z] });
                for (int g = 0; g < meshSequence.size(); g++) {
                    celestialShadowMapRenderStages[z]->drawMesh(meshSequence[g], 1);
                }
                celestialShadowMapRenderStages[z]->endDrawing();
                celestialShadowMapRenderStages[z]->submitNoSemaphores({});


                modelsShadowMapRenderStages[z]->beginDrawing();
                modelsShadowMapRenderStages[z]->setSets({ rendererDataSet, renderables[i]->shadowMapSet, shadowmapsDataSets[z] });

                scene->drawDrawableObjects(modelsShadowMapRenderStages[z], modelsDataSet, scale);

                modelsShadowMapRenderStages[z]->endDrawing();
                modelsShadowMapRenderStages[z]->submitNoSemaphores({});

                measureTimeEnd("Celestial shadow cascade " + std::to_string(z) + " data for " + std::to_string(i));
            }
        }
        */

        if (renderables[i]->getRenderMethod() == CelestialRenderMethod::lightAtmosphere) {
            measureTimeStart();
            celestialBodyWaterRenderStage->beginDrawing();

            celestialBodyWaterRenderStage->setSets({ rendererDataSet, renderables[i]->renderWaterSet });

            if (centerdist < radius * 7.0) {
                celestialBodyWaterRenderStage->drawMesh(subdividedMeshesProvider->getIcosphere(SubdividedMeshQuality::Medium), 1);
            }
            else {
                celestialBodyWaterRenderStage->drawMesh(subdividedMeshesProvider->getIcosphere(SubdividedMeshQuality::Low), 1);
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
        vulkan->waitDeviceIdle();
    }

    measureTimeStart();
    combineStage->beginDrawing();
    combineStage->setSets({ combineSet });
    combineStage->drawMesh(vulkan->getObject3dInfoFactory()->getFullScreenQuad(), 1);
    combineStage->endDrawing();
    combineStage->submitNoSemaphores({});
    measureTimeEnd("Composite output");

    outputScreenRenderer->draw();

#ifdef PERFORMANCE_DEBUG
    printf("\"nextframe\":true},");
#endif
}

VulkanDescriptorSetLayout * CosmosRenderer::getModelMRTLayout()
{
    return modelMRTLayout;
}

VulkanImage * CosmosRenderer::getUiOutputImage()
{
    return uiOutputImage;
}

void CosmosRenderer::onClosestStarChange(Star star)
{
    for (int i = 0; i < renderablePlanets.size(); i++) {
        delete renderablePlanets[i];
        renderablePlanets[i] = nullptr;
    }
    renderablePlanets.clear();


    for (auto planet : galaxy->getClosestStarPlanets()) {
        
        auto renderable = new RenderedCelestialBody(vulkan,
            planet,
            celestialBodyDataSetLayout,
            celestialShadowMapSetLayout,
            celestialBodyRenderSetLayout,
            celestialBodySurfaceSetLayout,
            celestialBodyWaterSetLayout,
            celestialBodyRaycastUniqueSetLayout,
            surfaceRenderedAlbedoRoughnessImage,
            surfaceRenderedNormalMetalnessImage,
            surfaceRenderedEmissionImage,
            surfaceRenderedDistanceImage,
            waterRenderedNormalMetalnessImage,
            waterRenderedDistanceImage);
        renderable->updateBuffer(observerCameraPosition, scale, 0.0);
        renderablePlanets.push_back(renderable);
        //renderable->updateData(celestialDataUpdateComputeStage);
            //renderable->updateData(celestialDataUpdateComputeStage);
    }
    
    //  vkDeviceWaitIdle(vulkan->device);
    //   vkDeviceWaitIdle(vulkan->device);
}

void CosmosRenderer::onClosestPlanetChange(CelestialBody planet)
{
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
            celestialBodyRaycastUniqueSetLayout,
            surfaceRenderedAlbedoRoughnessImage,
            surfaceRenderedNormalMetalnessImage,
            surfaceRenderedEmissionImage,
            surfaceRenderedDistanceImage,
            waterRenderedNormalMetalnessImage,
            waterRenderedDistanceImage);
        renderable->updateBuffer(observerCameraPosition, scale, 0.0);
        renderableMoons.push_back(renderable);
    }
}

void CosmosRenderer::measureTimeStart()
{
#ifdef PERFORMANCE_DEBUG
    vulkan->waitDeviceIdle();
    measurementStopwatch = glfwGetTime();
#endif
}

void CosmosRenderer::measureTimeEnd(std::string name)
{
#ifdef PERFORMANCE_DEBUG
    vulkan->waitDeviceIdle();
    double end = glfwGetTime();
    printf("    \"%s\": %f,\n", name.c_str(), 1000.0 * (end - measurementStopwatch));
#endif
}

GalaxyContainer * CosmosRenderer::getGalaxy()
{
    return galaxy;
}
double CosmosRenderer::getExposure()
{
    return exposure;
}
void CosmosRenderer::setExposure(double value)
{
    exposure = value;
}

void CosmosRenderer::setRaycastPoints(std::vector<glm::dvec3> points)
{
    raycastPoints = points;
    auto bb = VulkanBinaryBufferBuilder();
    bb.emplaceInt32(static_cast<int32_t>(raycastPoints.size()));
    bb.emplaceInt32(static_cast<int32_t>(raycastPoints.size()));
    bb.emplaceInt32(static_cast<int32_t>(raycastPoints.size()));
    bb.emplaceInt32(static_cast<int32_t>(raycastPoints.size()));
    for (auto point : raycastPoints) {
        bb.emplaceFloat32((point.x - observerCameraPosition.x) * scale);
        bb.emplaceFloat32((point.y - observerCameraPosition.y) * scale);
        bb.emplaceFloat32((point.z - observerCameraPosition.z) * scale);
        bb.emplaceFloat32(0.0f);
    }
    void* data;
    raycastRequestsDataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    raycastRequestsDataBuffer->unmap();
}

std::vector<glm::dvec3> CosmosRenderer::getRaycastPoints()
{
    return raycastPoints;
}

VulkanImage * CosmosRenderer::getOpaqueSurfaceDistanceImage()
{
    return surfaceRenderedDistanceImage;
}

RenderedCelestialBody * CosmosRenderer::getRenderableForCelestialBody(CelestialBody body)
{
    for (auto renderable : renderablePlanets) {
        if (renderable->body.bodyId == body.bodyId) {
            return renderable;
        }
    }
    for (auto renderable : renderableMoons) {
        if (renderable->body.bodyId == body.bodyId) {
            return renderable;
        }
    }
    return nullptr;
}
