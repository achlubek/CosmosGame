#include "stdafx.h"
#include "OutputScreenRenderer.h"


OutputScreenRenderer::OutputScreenRenderer(VulkanToolkit* ivulkan, int iwidth, int iheight)
    : vulkan(ivulkan), width(iwidth), height(iheight)
{
    layout = new VulkanDescriptorSetLayout(vulkan);
    layout->addField(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    layout->addField(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    layout->compile();

    set = layout->generateDescriptorSet();


    auto vertShader = new VulkanShaderModule(vulkan, "../../shaders/compiled/output-screen.vert.spv");
    auto fragShader = new VulkanShaderModule(vulkan, "../../shaders/compiled/output-screen.frag.spv");

    renderStage = new VulkanRenderStage(vulkan);
    renderStage->setViewport(vulkan->windowWidth, vulkan->windowHeight);
    renderStage->addShaderStage(vertShader->createShaderStage(VK_SHADER_STAGE_VERTEX_BIT, "main"));
    renderStage->addShaderStage(fragShader->createShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, "main"));
    renderStage->addDescriptorSetLayout(layout->layout);
    renderStage->setSets({ set });

    renderer = new VulkanRenderer(vulkan);
    renderer->setOutputStage(renderStage);
    renderer->compile();
}


OutputScreenRenderer::~OutputScreenRenderer()
{
}

void OutputScreenRenderer::draw(VulkanImage * image, VulkanImage * uiImage)
{
    if (image != lastImage || uiImage != lastUiImage) {
        rebuildDescriptorSet(image, uiImage);
        lastImage = image;
        lastUiImage = uiImage;
    }
    renderer->beginDrawing();
    renderer->endDrawing();
}

void OutputScreenRenderer::rebuildDescriptorSet(VulkanImage * image, VulkanImage * uiImage)
{
    set->bindImageViewSampler(0, image);
    set->bindImageViewSampler(1, uiImage);
    set->update();
}
