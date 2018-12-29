#include "stdafx.h"
#include "StarsRenderer.h"


StarsRenderer::StarsRenderer(ToolkitInterface* toolkit, 
    int width, int height, double scale,
    DescriptorSetLayoutInterface* rendererDataSetLayout,
    DescriptorSetInterface* rendererDataSet,
    GalaxyContainer* galaxy)
    : toolkit(toolkit),
    width(width), 
    height(height), 
    scale(scale), 
    rendererDataSetLayout(rendererDataSetLayout),
    rendererDataSet(rendererDataSet),
    galaxy(galaxy)
{
    starsImage = toolkit->getImageFactory()->build(width, height, VEngineImageFormat::RGBA16f, VEngineImageUsage::ColorAttachment | VEngineImageUsage::Storage | VEngineImageUsage::Sampled);

    starsDataBuffer = toolkit->getBufferFactory()->build(VEngineBufferType::BufferTypeStorage, 1024 * 1024 * 128);

    starsDataLayout = toolkit->getDescriptorSetLayoutFactory()->build();
    starsDataLayout->addField(VEngineDescriptorSetFieldType::FieldTypeStorageBuffer, VEngineDescriptorSetFieldStage::FieldStageAllGraphics);
    
    starsDataSet = starsDataLayout->generateDescriptorSet();
    starsDataSet->bindBuffer(0, starsDataBuffer);

    createRenderStage();

    cube3dInfo = toolkit->getObject3dInfoFactory()->build("cube1unitradius.raw");

    updateStarsBuffer();

    doesNeedRecording = true;
}


#define safedelete(a) if(a!=nullptr){delete a;a=nullptr;}
StarsRenderer::~StarsRenderer()
{
    safedelete(cube3dInfo);
    safedelete(starsStage);
    safedelete(starsDataSet);
    safedelete(starsDataLayout);
    safedelete(starsDataBuffer);
    safedelete(starsImage);
}

void StarsRenderer::draw(std::vector<SemaphoreInterface*> waitSemaphores)
{
    if (doesNeedRecording) {
        starsStage->beginDrawing();

        starsStage->drawMesh(cube3dInfo, static_cast<uint32_t>(galaxy->getStarsCount()));

        starsStage->endDrawing();
        doesNeedRecording = false;
    }
    starsStage->submit(waitSemaphores);
}

void StarsRenderer::recompile()
{
    safedelete(starsStage);
    createRenderStage();
    doesNeedRecording = true;
}

ImageInterface * StarsRenderer::getStarsImage()
{
    return starsImage;
}

SemaphoreInterface * StarsRenderer::getSignalSemaphore()
{
    return starsStage->getSignalSemaphore();
}

void StarsRenderer::updateStarsBuffer()
{
    BinaryBufferBuilder starsBB = BinaryBufferBuilder();
    auto stars = galaxy->getAllStars();

    for (int s = 0; s < stars.size(); s++) {
        auto star = stars[s];

        glm::dvec3 starpos = star.getPosition(0.0) * scale;

        starsBB.emplaceFloat32(starpos.x);
        starsBB.emplaceFloat32(starpos.y);
        starsBB.emplaceFloat32(starpos.z);
        starsBB.emplaceFloat32(star.radius * scale);

        starsBB.emplaceFloat32(star.color.x);
        starsBB.emplaceFloat32(star.color.y);
        starsBB.emplaceFloat32(star.color.z);
        starsBB.emplaceFloat32(star.age);
    }
    void* data;
    starsDataBuffer->map(0, starsBB.buffer.size(), &data);
    memcpy(data, starsBB.getPointer(), starsBB.buffer.size());
    starsDataBuffer->unmap();
}

void StarsRenderer::createRenderStage()
{
    auto starsvert = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Vertex, "cosmos-stars.vert.spv");
    auto starsfrag = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Fragment, "cosmos-stars.frag.spv");

    starsStage = toolkit->getRenderStageFactory()->build(width, height, { starsvert, starsfrag }, { rendererDataSetLayout, starsDataLayout },
    {
        starsImage->getAttachment(VEngineAttachmentBlending::Additive, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } })
    }, VEngineCullMode::CullModeBack);
    starsStage->setSets({ rendererDataSet, starsDataSet });
}
