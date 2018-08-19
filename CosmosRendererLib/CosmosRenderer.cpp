#include "stdafx.h"
#include "CosmosRenderer.h"
#include "SceneProvider.h"
#include "GalaxyContainer.h"
#include "AbsGameContainer.h"
#include "ModelsRenderer.h"
#include "TimeProvider.h"
#include "ParticlesRenderer.h"
#include "AbsGameStage.h"
#include "StarsRenderer.h"


CosmosRenderer::CosmosRenderer(VulkanToolkit* vulkan, GalaxyContainer* galaxy, int width, int height) :
    galaxy(galaxy), width(width), height(height),
    vulkan(vulkan), renderablePlanets({}), renderableMoons({}), updatingSafetyQueue(InvokeQueue())
{
    //  internalCamera = new Camera();

    cube3dInfo = vulkan->getObject3dInfoFactory()->build("cube1unitradius.raw");

    auto wholeIcoMesh = vulkan->getObject3dInfoFactory()->build("icosphere_to_separate.raw");
    auto splitMesh = splitTriangles(wholeIcoMesh);
    for (int i = 0; i < splitMesh.size(); i++) {
        auto vbo = splitMesh[i]->getVBO();
        int g = 0;
        glm::vec3 v1 = glm::normalize(glm::vec3(vbo[g], vbo[g + 1], vbo[g + 2]));
        g += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(vbo[g], vbo[g + 1], vbo[g + 2]));
        g += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(vbo[g], vbo[g + 1], vbo[g + 2]));

        glm::vec3 dir = glm::normalize((v1 + v2 + v3) / glm::vec3(3.0));
        auto low = subdivide(splitMesh[i]);
        auto medium = subdivide(low);
        auto high = subdivide(subdivide(medium));
        patchesLowPoly.push_back({ dir, low });
        patchesMediumPoly.push_back({ dir, medium });
        patchesHighPoly.push_back({ dir, high });
    }


    icosphereLow = vulkan->getObject3dInfoFactory()->build("icosphere_mediumpoly_1unit.raw");

    icosphereMedium = vulkan->getObject3dInfoFactory()->build("icosphere_mediumpoly_1unit.raw");

    icosphereHigh = subdivide(icosphereMedium);// vulkan->getObject3dInfoFactory->build("icosphere_highpoly_1unit.raw");

    cameraDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 1024);
    planetsDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, sizeof(float) * 1024 * 1024);
    moonsDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, sizeof(float) * 1024 * 1024);

    celestialAlphaImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Storage | VulkanImageUsage::Sampled);

    celestialAdditiveImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    //####################//

    surfaceRenderedAlbedoRoughnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA8unorm, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    surfaceRenderedNormalMetalnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    surfaceRenderedDistanceImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    surfaceRenderedDepthImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth);

    //#######//

    waterRenderedNormalMetalnessImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    waterRenderedDistanceImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled);

    waterRenderedDepthImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth);

    //#########//

    shadowmapsDepthMap = vulkan->getVulkanImageFactory()->build(shadowMapWidth, shadowMapHeight, VulkanImageFormat::Depth32f, VulkanImageUsage::Depth);

    shadowMapDataSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    shadowMapDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);

    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        shadowmaps.push_back(vulkan->getVulkanImageFactory()->build(shadowMapWidth, shadowMapHeight, VulkanImageFormat::R32f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Sampled));
        auto buff = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeUniform, sizeof(float) * 1024);
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

    celestialBodyDataSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);

    celestiaStarsBlitSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestiaStarsBlitSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestiaStarsBlitSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);

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
    combineLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    combineLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    combineLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    combineLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    combineSet = combineLayout->generateDescriptorSet();
    combineSet->bindBuffer(0, cameraDataBuffer);
    combineSet->bindImageViewSampler(1, celestialAlphaImage);
    combineSet->bindImageViewSampler(2, starsRenderer->getStarsImage());
    combineSet->bindImageViewSampler(3, celestialAdditiveImage);
    combineSet->bindImageViewSampler(4, AbsGameContainer::getInstance()->getModelsRenderer()->getAlbedoRoughnessImage());
    combineSet->bindImageViewSampler(5, AbsGameContainer::getInstance()->getModelsRenderer()->getNormalMetalnessImage());
    combineSet->bindImageViewSampler(6, AbsGameContainer::getInstance()->getModelsRenderer()->getDistanceImage());
    combineSet->bindImageViewSampler(7, AbsGameContainer::getInstance()->getParticlesRenderer()->getResultImage());


    shadowMapsCollectionSet = shadowMapsCollectionLayout->generateDescriptorSet();
    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        shadowMapsCollectionSet->bindImageViewSampler(i, shadowmaps[i]);
    }

    celestiaStarsBlitSet = celestiaStarsBlitSetLayout->generateDescriptorSet();
    celestiaStarsBlitSet->bindImageViewSampler(0, starsRenderer->getStarsImage());
    celestiaStarsBlitSet->bindImageStorage(1, celestialAlphaImage);

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

    for (int i = 0; i < shadowmapsDivisors.size(); i++) {
        auto celestialShadowMapRenderStage = vulkan->getVulkanRenderStageFactory()->build(shadowMapWidth, shadowMapHeight,
            { celestialshadowmapvert, celestialshadowmapfrag }, { rendererDataLayout, celestialShadowMapSetLayout, shadowMapDataSetLayout },
            {
                shadowmaps[i]->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
                shadowmapsDepthMap->getAttachment(VulkanAttachmentBlending::None)
            });
        celestialShadowMapRenderStages.push_back(celestialShadowMapRenderStage);
    }

    //**********************//

    auto celestialsurfacevert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-celestial-surface.vert.spv");
    auto celestialsurfacefrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-celestial-surface.frag.spv");

    celestialBodySurfaceRenderStage = vulkan->getVulkanRenderStageFactory()->build(width, height, 
        { celestialsurfacevert, celestialsurfacefrag }, { rendererDataLayout, celestialBodySurfaceSetLayout },
        {
            surfaceRenderedAlbedoRoughnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            surfaceRenderedNormalMetalnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            surfaceRenderedDistanceImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            surfaceRenderedDepthImage->getAttachment(VulkanAttachmentBlending::None)
        }, VulkanCullMode::CullModeFront);

    //**********************//

    auto celestialwatervert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-celestial-water.vert.spv");
    auto celestialwaterfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-celestial-water.frag.spv");

    celestialBodyWaterRenderStage = vulkan->getVulkanRenderStageFactory()->build(width, height,
        { celestialwatervert, celestialwaterfrag }, { rendererDataLayout, celestialBodyWaterSetLayout },
        {
            waterRenderedNormalMetalnessImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            waterRenderedDistanceImage->getAttachment(VulkanAttachmentBlending::None, true,{ { 0.0f, 0.0f, 0.0f, 0.0f } }),
            waterRenderedDepthImage->getAttachment(VulkanAttachmentBlending::None)
        });

    //**********************//
    auto celestialdatacompute = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "celestial-updatedata.comp.spv");

    celestialDataUpdateComputeStage = vulkan->getVulkanComputeStageFactory()->build(celestialdatacompute, { celestialBodyDataSetLayout });

    //**********************//
    auto celestialblitcompute = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "celestial-blit-stars.comp.spv");

    celestialStarsBlitComputeStage = vulkan->getVulkanComputeStageFactory()->build(celestialblitcompute, { celestiaStarsBlitSetLayout });

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
            AbsGameContainer::getInstance()->getOutputImage()->getAttachment(VulkanAttachmentBlending::None, true),
        });
    combineStage->setSets({ combineSet });

    //**********************//

    if (deleteOld) {
        onClosestPlanetChange(galaxy->getClosestPlanet());
    }

    readyForDrawing = true;
    firstRecordingDone = false;
}

void CosmosRenderer::mapBuffers()
{
    planetsDataBuffer->map(0, planetsDataBuffer->getSize(), &planetsDataBufferPointer);
    moonsDataBuffer->map(0, moonsDataBuffer->getSize(), &moonsDataBufferPointer);
}

void CosmosRenderer::unmapBuffers()
{
    planetsDataBuffer->unmap();
    moonsDataBuffer->unmap();
}

void CosmosRenderer::updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition, double time)
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();
    double xpos, ypos;
    auto cursorpos = vulkan->getMouse()->getCursorPosition();
    xpos = std::get<0>(cursorpos);
    ypos = std::get<1>(cursorpos);

    glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    glm::mat4 vpmatrix = clip * camera->projectionMatrix * camera->transformation->getInverseWorldTransform();

    glm::mat4 cameraViewMatrix = camera->transformation->getInverseWorldTransform();
    glm::mat4 cameraRotMatrix = camera->transformation->getRotationMatrix();
    glm::mat4 rpmatrix = camera->projectionMatrix * inverse(cameraRotMatrix);
    // camera->cone->update(inverse(rpmatrix));

    auto star = galaxy->getClosestStar();
    glm::dvec3 closesStarRelPos = (star.getPosition(time) - observerPosition) * scale;

    bb.emplaceFloat32((float)time);
    bb.emplaceFloat32(0.0f);
    bb.emplaceFloat32((float)xpos / (float)width);
    bb.emplaceFloat32((float)ypos / (float)height);
    bb.emplaceGeneric((unsigned char*)&rpmatrix, sizeof(rpmatrix));

    glm::vec3 newcamerapos = glm::vec3(observerPosition * scale);
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
    double au1 = 1496000.0;
    double aucoeff = (glm::distance(observerPosition, galaxy->getClosestStar().getPosition(0))) / au1;
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

void CosmosRenderer::draw(double time)
{
    if (!readyForDrawing) return;

#ifdef PERFORMANCE_DEBUG
    printf("{");
#endif
    //galaxy->update(observerCameraPosition);

    measureTimeStart();
    updatingSafetyQueue.executeAll();
    measureTimeEnd("Executing safety queue");

    measureTimeStart();

    starsRenderer->draw();

    measureTimeEnd("Stars galaxy draw");

    //vkDeviceWaitIdle(vulkan->device);

    measureTimeStart();
    if (!firstRecordingDone) {
        celestialStarsBlitComputeStage->beginRecording();
        celestialStarsBlitComputeStage->dispatch({ celestiaStarsBlitSet }, width / 256 + 1, height / 2, 1);
        celestialStarsBlitComputeStage->endRecording();
    }
    firstRecordingDone = true;

    celestialStarsBlitComputeStage->submitNoSemaphores({  });

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
            renderables[a]->resizeDataImages(256, 256, 256, 256);
        }
        renderables[renderables.size() - 1]->resizeDataImages(2048, 2048, 1024, 1024);

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
            // for (int z = 0; z < shadowmapsDivisors.size(); z++) {
            int z = cascadeCounter;
            cascadeCounter++;
            if (cascadeCounter >= shadowmapsDivisors.size()) {
                cascadeCounter = 0;
            }
            measureTimeStart();
            celestialShadowMapRenderStages[z]->beginDrawing();
            celestialShadowMapRenderStages[z]->setSets({ rendererDataSet, renderables[i]->shadowMapSet, shadowmapsDataSets[z] });
            for (int g = 0; g < meshSequence.size(); g++) {
                celestialShadowMapRenderStages[z]->drawMesh(meshSequence[g], 1);
            }
            celestialShadowMapRenderStages[z]->endDrawing();
            celestialShadowMapRenderStages[z]->submitNoSemaphores({});
            measureTimeEnd("Celestial shadow cascade " + std::to_string(z) + " data for " + std::to_string(i));
            // }
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
        vulkan->waitDeviceIdle();
    }

    measureTimeStart();
    combineStage->beginDrawing();
    combineStage->setSets({ combineSet });
    combineStage->drawMesh(vulkan->getObject3dInfoFactory()->getFullScreenQuad(), 1);
    combineStage->endDrawing();
    combineStage->submitNoSemaphores({});
    measureTimeEnd("Composite output");

#ifdef PERFORMANCE_DEBUG
    printf("\"nextframe\":true},");
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
        renderable->updateBuffer(observerCameraPosition, scale, 0.0);
        renderablePlanets.push_back(renderable);
        //renderable->updateData(celestialDataUpdateComputeStage);

        vulkan->waitDeviceIdle();

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
            renderable->updateBuffer(observerCameraPosition, scale, 0.0);
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
    printf("    \"%s\": %f,\n", name.c_str(), 1000.0 * (end - measurementStopwatch));
#endif
}

Object3dInfo * CosmosRenderer::subdivide(Object3dInfo * info)
{
    std::vector<float> floats = {};
    auto vbo = info->getVBO();
    for (int i = 0; i < vbo.size();) {
        glm::vec3 v1 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
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
    return vulkan->getObject3dInfoFactory()->build(floats);
}

std::vector<Object3dInfo*> CosmosRenderer::splitTriangles(Object3dInfo * info)
{
    std::vector<Object3dInfo*> objs = {};

    auto vbo = info->getVBO();
    for (int i = 0; i < vbo.size();) {
        glm::vec3 v1 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 v2 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
        i += 12;
        glm::vec3 v3 = glm::normalize(glm::vec3(vbo[i], vbo[i + 1], vbo[i + 2]));
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
        objs.push_back(vulkan->getObject3dInfoFactory()->build(buffer));
    }
    return objs;
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
void CosmosRenderer::invokeOnDrawingThread(std::function<void(void)> func)
{
    updatingSafetyQueue.enqueue(func);
}