#include "stdafx.h"
#include "OutputScreenRenderer.h"


OutputScreenRenderer::OutputScreenRenderer(VulkanToolkit* vulkan, int width, int height,
    VulkanImage * image, VulkanImage * uiImage)
    : vulkan(vulkan), width(width), height(height)
{
    layout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    layout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);
    layout->addField(VulkanDescriptorSetFieldType::FieldTypeSampler, VulkanDescriptorSetFieldStage::FieldStageFragment);

    set = layout->generateDescriptorSet();
    set->bindImageViewSampler(0, image);
    set->bindImageViewSampler(1, uiImage);

    auto vertShader = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Vertex, "output-screen.vert.spv");
    auto fragShader = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Fragment, "output-screen.frag.spv");

    renderStage = vulkan->getVulkanRenderStageFactory()->build(width, height, { vertShader, fragShader }, { layout }, {});
    renderStage->setSets({ set });

    swapChainOutput = vulkan->getVulkanSwapChainOutputFactory()->build(renderStage);
}


OutputScreenRenderer::~OutputScreenRenderer()
{
}

void OutputScreenRenderer::draw()
{
    swapChainOutput->beginDrawing();
    swapChainOutput->drawMesh(vulkan->getObject3dInfoFactory()->getFullScreenQuad(), 1);
    swapChainOutput->endDrawing();
}
