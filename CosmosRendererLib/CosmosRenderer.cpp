#include "stdafx.h"
#include "CosmosRenderer.h"


CosmosRenderer::CosmosRenderer(VulkanToolkit* vulkan, EventBus * eventBus, GalaxyContainer* galaxy, int width, int height) :
    galaxy(galaxy), width(width), height(height),
    vulkan(vulkan), renderablePlanets({}), renderableMoons({}), eventBus(eventBus),
    subdividedMeshesProvider(new SubdividedMeshesProvider(vulkan))
{
    //  internalCamera = new Camera();

    celestialBodyDataUpdater = new CelestialBodyDataUpdater(vulkan);

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


    //####################//

    rendererDataLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    rendererDataLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAll);

    rendererDataSet = rendererDataLayout->generateDescriptorSet();
    rendererDataSet->bindBuffer(0, cameraDataBuffer);

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
    celestialBodyRaycastUniqueSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);
    
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

    celestialBodySurfaceSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodySurfaceSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    celestialBodySurfaceSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    celestialBodySurfaceSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);

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
    
    celestiaStarsBlitSet = celestialStarsBlitSetLayout->generateDescriptorSet();
    celestiaStarsBlitSet->bindImageViewSampler(0, starsRenderer->getStarsImage());
    celestiaStarsBlitSet->bindImageStorage(1, celestialAlphaImage);

    recompileShaders(false);

    eventBus->registerHandler(new OnClosestStarChangeEventHandler(this));
    eventBus->registerHandler(new OnClosestPlanetChangeEventHandler(this));
    eventBus->registerHandler(new OnClosestMoonChangeEventHandler(this));

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
        safedelete(celestialStage);
        safedelete(combineStage);
    }
    vulkan->waitDeviceIdle();
    
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

    auto celestialdataraycast = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "celestial-raycast.comp.spv");

    celestialBodyRaycastComputeStage = vulkan->getVulkanComputeStageFactory()->build(celestialdataraycast, { celestialBodyRaycastSharedSetLayout, celestialBodyRaycastUniqueSetLayout });

    //**********************//
    auto celestialblitcompute = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "celestial-blit-stars.comp.spv");

    celestialStarsBlitComputeStage = vulkan->getVulkanComputeStageFactory()->build(celestialblitcompute, { celestialStarsBlitSetLayout });

    //**********************//

    auto celestialvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-celestial.vert.spv");
    auto celestialfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-celestial.frag.spv");

    celestialStage = vulkan->getVulkanRenderStageFactory()->build(width, height,
        { celestialvert, celestialfrag }, { rendererDataLayout, celestialBodyRenderSetLayout },
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
    eventBus->processQueue();

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
    }

    celestialBodyDataUpdater->update(renderables);

    measureTimeEnd("Preparing for celestial");
     /*
    if (raycastPoints.size() > 0) {
        celestialBodyRaycastComputeStage->beginRecording();
        for (int a = 0; a < renderables.size(); a++) {
            renderables[a]->updateRaycasts(static_cast<uint32_t>(raycastPoints.size()), celestialBodyRaycastSharedSet, celestialBodyRaycastComputeStage);
        }
        celestialBodyRaycastComputeStage->endRecording();
        celestialBodyRaycastComputeStage->submitNoSemaphores({});
    }
    */
    for (int a = 0; a < renderables.size(); a++) {
        renderables[a]->updateBuffer(observerCameraPosition, scale, time);
    } 

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
        }
        else {
            meshSequence.push_back(subdividedMeshesProvider->getIcosphere(SubdividedMeshQuality::High));
        }

        for (int g = 0; g < meshSequence.size(); g++) {
            celestialBodySurfaceRenderStage->drawMesh(meshSequence[g], 1);
        }

        celestialBodySurfaceRenderStage->endDrawing();
        celestialBodySurfaceRenderStage->submitNoSemaphores({  });

        measureTimeEnd("Celestial surface data for " + std::to_string(i));
        
        measureTimeStart();

        celestialStage->beginDrawing();

        renderables[i]->draw(celestialStage, rendererDataSet, cube3dInfo);

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
            celestialBodyDataUpdater->getBodyDataSetLayout(),
            celestialBodyRenderSetLayout,
            celestialBodySurfaceSetLayout,
            celestialBodyRaycastUniqueSetLayout,
            surfaceRenderedAlbedoRoughnessImage,
            surfaceRenderedNormalMetalnessImage,
            surfaceRenderedEmissionImage,
            surfaceRenderedDistanceImage);
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
            celestialBodyDataUpdater->getBodyDataSetLayout(),
            celestialBodyRenderSetLayout,
            celestialBodySurfaceSetLayout,
            celestialBodyRaycastUniqueSetLayout,
            surfaceRenderedAlbedoRoughnessImage,
            surfaceRenderedNormalMetalnessImage,
            surfaceRenderedEmissionImage,
            surfaceRenderedDistanceImage);
        renderable->updateBuffer(observerCameraPosition, scale, 0.0);
        renderableMoons.push_back(renderable);
    }
}

void CosmosRenderer::onClosestMoonChange(CelestialBody moon)
{
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
