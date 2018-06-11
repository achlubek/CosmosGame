#include "stdafx.h"
#include "StarsRenderer.h"
#include "AssetLoader.h"
#include "GalaxyContainer.h"


StarsRenderer::StarsRenderer(VulkanToolkit* vulkan, 
    int width, int height, double scale,
    VulkanDescriptorSet* rendererDataSet, 
    AssetLoader* assetLoader, 
    GalaxyContainer* galaxy)
    : vulkan(vulkan), 
    width(width), 
    height(height), 
    scale(scale), 
    rendererDataSet(rendererDataSet),
    assetLoader(assetLoader),
    galaxy(galaxy)
{
    starsImage = new VulkanImage(vulkan, width, height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, false);
    
    starsDataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 1024 * 1024 * 128);

    starsDataLayout = new VulkanDescriptorSetLayout(vulkan);
    starsDataLayout->addField(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS);
    starsDataLayout->compile();    
    
    starsDataSet = starsDataLayout->generateDescriptorSet();
    starsDataSet->bindStorageBuffer(0, starsDataBuffer);
    starsDataSet->update();

    createRenderStage();

    cube3dInfo = assetLoader->loadObject3dInfoFile("cube1unitradius.raw");

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
    auto starsvert = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-stars.vert.spv");
    auto starsfrag = new VulkanShaderModule(vulkan, "../../shaders/compiled/cosmos-stars.frag.spv");

    starsStage = new VulkanRenderStage(vulkan);
    starsStage->setViewport(width, height);
    starsStage->addShaderStage(starsvert->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    starsStage->addShaderStage(starsfrag->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    starsStage->addDescriptorSetLayout(rendererDataSet->layout);
    starsStage->addDescriptorSetLayout(starsDataLayout->layout);
    starsStage->addOutputImage(starsImage);
    starsStage->setSets({ rendererDataSet, starsDataSet });
    starsStage->additiveBlending = true;
    starsStage->cullFlags = VK_CULL_MODE_BACK_BIT;
    starsStage->compile();
}
