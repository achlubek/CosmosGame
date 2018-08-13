#include "stdafx.h"
#include "StarsRenderer.h"
#include "GalaxyContainer.h"


StarsRenderer::StarsRenderer(VulkanToolkit* vulkan, 
    int width, int height, double scale,
    VulkanDescriptorSetLayout* rendererDataSetLayout,
    VulkanDescriptorSet* rendererDataSet, 
    GalaxyContainer* galaxy)
    : vulkan(vulkan), 
    width(width), 
    height(height), 
    scale(scale), 
    rendererDataSetLayout(rendererDataSetLayout),
    rendererDataSet(rendererDataSet),
    galaxy(galaxy)
{
    starsImage = vulkan->getVulkanImageFactory()->build(width, height, VulkanImageFormat::RGBA16f, VulkanImageUsage::ColorAttachment | VulkanImageUsage::Storage | VulkanImageUsage::Sampled);

    starsDataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, 1024 * 1024 * 128);

    starsDataLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    starsDataLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageBuffer, VulkanDescriptorSetFieldStage::FieldStageAllGraphics);
    
    starsDataSet = starsDataLayout->generateDescriptorSet();
    starsDataSet->bindBuffer(0, starsDataBuffer);

    createRenderStage();

    cube3dInfo = vulkan->getObject3dInfoFactory()->build("cube1unitradius.raw");

    updateStarsBuffer();

    doesNeedsRecording = true;
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

void StarsRenderer::draw()
{
    if (doesNeedsRecording) {
        starsStage->beginDrawing();

        starsStage->drawMesh(cube3dInfo, galaxy->getStarsCount());

        starsStage->endDrawing();
        doesNeedsRecording = false;
    }
    starsStage->submitNoSemaphores({});
}

void StarsRenderer::recompile()
{
    safedelete(starsStage);
    updateStarsBuffer();
}

VulkanImage * StarsRenderer::getStarsImage()
{
    return starsImage;
}

void StarsRenderer::updateStarsBuffer()
{
    VulkanBinaryBufferBuilder starsBB = VulkanBinaryBufferBuilder();
    auto stars = galaxy->getAllStars();
    starsBB.emplaceInt32(stars.size());
    starsBB.emplaceInt32(stars.size());
    starsBB.emplaceInt32(stars.size());
    starsBB.emplaceInt32(stars.size());
    for (int s = 0; s < stars.size(); s++) {
        auto star = stars[s];

        glm::dvec3 starpos = star.getPosition(0.0) * scale;

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
    void* data;
    starsDataBuffer->map(0, starsBB.buffer.size(), &data);
    memcpy(data, starsBB.getPointer(), starsBB.buffer.size());
    starsDataBuffer->unmap();
}

void StarsRenderer::createRenderStage()
{
    auto starsvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-stars.vert.spv");
    auto starsfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-stars.frag.spv");

    starsStage = vulkan->getVulkanRenderStageFactory()->build(width, height, { starsvert, starsfrag }, { rendererDataSetLayout, starsDataLayout },
    {
        starsImage->getAttachment(VulkanAttachmentBlending::Additive, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } })
    }, VulkanCullMode::CullModeBack);
    starsStage->setSets({ rendererDataSet, starsDataSet });
}
