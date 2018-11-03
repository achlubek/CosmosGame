#include "stdafx.h"
#include "StarsRenderer.h"


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

void StarsRenderer::draw(std::vector<VkSemaphore> waitSemaphores)
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

VulkanImage * StarsRenderer::getStarsImage()
{
    return starsImage;
}

VkSemaphore StarsRenderer::getSignalSemaphore()
{
    return starsStage->getSignalSemaphore();
}

void StarsRenderer::updateStarsBuffer()
{
    VulkanBinaryBufferBuilder starsBB = VulkanBinaryBufferBuilder();
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
    auto starsvert = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "cosmos-stars.vert.spv");
    auto starsfrag = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "cosmos-stars.frag.spv");

    starsStage = vulkan->getVulkanRenderStageFactory()->build(width, height, { starsvert, starsfrag }, { rendererDataSetLayout, starsDataLayout },
    {
        starsImage->getAttachment(VulkanAttachmentBlending::Additive, true,{ { 0.0f, 0.0f, 0.0f, 1.0f } })
    }, VulkanCullMode::CullModeBack);
    starsStage->setSets({ rendererDataSet, starsDataSet });
}
