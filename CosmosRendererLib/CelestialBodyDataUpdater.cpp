#include "stdafx.h"
#include "CelestialBodyDataUpdater.h"


CelestialBodyDataUpdater::CelestialBodyDataUpdater(VulkanToolkit* vulkan)
{
    celestialBodyDataSetLayout = vulkan->getVulkanDescriptorSetLayoutFactory()->build();
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeUniformBuffer, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VulkanDescriptorSetFieldType::FieldTypeStorageImage, VulkanDescriptorSetFieldStage::FieldStageCompute);

    auto celestialdatacompute = vulkan->getVulkanShaderFactory()->build(VulkanShaderModuleType::Compute, "celestial-updatedata.comp.spv");

    celestialDataUpdateComputeStage = vulkan->getVulkanComputeStageFactory()->build(celestialdatacompute, { celestialBodyDataSetLayout });
}


CelestialBodyDataUpdater::~CelestialBodyDataUpdater()
{
}

VulkanDescriptorSetLayout * CelestialBodyDataUpdater::getBodyDataSetLayout()
{
    return celestialBodyDataSetLayout;
}

void CelestialBodyDataUpdater::update(std::vector<RenderedCelestialBody*> renderables)
{
    for (int a = 0; a < renderables.size() - 1; a++) {
        renderables[a]->resizeDataImages(128, 128, 128, 128);
    }
    renderables[renderables.size() - 1]->resizeDataImages(1024, 1024, 1024, 1024);

    celestialDataUpdateComputeStage->beginRecording();
    for (int a = 0; a < renderables.size(); a++) {
        if (renderables[a]->needsDataUpdate()) {
            renderables[a]->updateData(celestialDataUpdateComputeStage);
        }
    }
    celestialDataUpdateComputeStage->endRecording();
    celestialDataUpdateComputeStage->submitNoSemaphores({});
}
