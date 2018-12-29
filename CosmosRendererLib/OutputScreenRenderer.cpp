#include "stdafx.h"
#include "OutputScreenRenderer.h"


OutputScreenRenderer::OutputScreenRenderer(ToolkitInterface* toolkit, int width, int height,
    ImageInterface * image, ImageInterface * uiImage)
    : toolkit(toolkit), width(width), height(height)
{
    layout = toolkit->getDescriptorSetLayoutFactory()->build();
    layout->addField(VEngineDescriptorSetFieldType::FieldTypeSampler, VEngineDescriptorSetFieldStage::FieldStageFragment);
    layout->addField(VEngineDescriptorSetFieldType::FieldTypeSampler, VEngineDescriptorSetFieldStage::FieldStageFragment);

    set = layout->generateDescriptorSet();
    set->bindImageViewSampler(0, image);
    set->bindImageViewSampler(1, uiImage);

    auto vertShader = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Vertex, "output-screen.vert.spv");
    auto fragShader = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Fragment, "output-screen.frag.spv");

    renderStage = toolkit->getRenderStageFactory()->build(width, height, { vertShader, fragShader }, { layout }, {});
    renderStage->setSets({ set });

    swapChainOutput = toolkit->getSwapChainOutputFactory()->build(renderStage);
}


OutputScreenRenderer::~OutputScreenRenderer()
{
}

void OutputScreenRenderer::draw()
{
    swapChainOutput->beginDrawing();
    swapChainOutput->drawMesh(toolkit->getObject3dInfoFactory()->getFullScreenQuad(), 1);
    swapChainOutput->endDrawing();
    swapChainOutput->submit({ });
}
