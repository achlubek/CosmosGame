#include "stdafx.h"
#include "CelestialBodyDataUpdater.h"


CelestialBodyDataUpdater::CelestialBodyDataUpdater(ToolkitInterface* toolkit)
{
    celestialBodyDataSetLayout = toolkit->getDescriptorSetLayoutFactory()->build();
    celestialBodyDataSetLayout->addField(VEngineDescriptorSetFieldType::FieldTypeUniformBuffer, VEngineDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VEngineDescriptorSetFieldType::FieldTypeStorageImage, VEngineDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VEngineDescriptorSetFieldType::FieldTypeStorageImage, VEngineDescriptorSetFieldStage::FieldStageCompute);
    celestialBodyDataSetLayout->addField(VEngineDescriptorSetFieldType::FieldTypeStorageImage, VEngineDescriptorSetFieldStage::FieldStageCompute);

    auto celestialdatacompute = toolkit->getShaderFactory()->build(VEngineShaderModuleType::Compute, "celestial-updatedata.comp.spv");

    celestialDataUpdateComputeStage = toolkit->getComputeStageFactory()->build(celestialdatacompute, { celestialBodyDataSetLayout });
}


CelestialBodyDataUpdater::~CelestialBodyDataUpdater()
{
}

DescriptorSetLayoutInterface * CelestialBodyDataUpdater::getBodyDataSetLayout()
{
    return celestialBodyDataSetLayout;
}

void CelestialBodyDataUpdater::update(std::vector<RenderedCelestialBody*> renderables)
{
    for (int a = 0; a < renderables.size() - 1; a++) {
        renderables[a]->resizeDataImages(128, 128, 128, 128);
    }
    renderables[renderables.size() - 1]->resizeDataImages(2048, 2048, 2048, 2048);

    celestialDataUpdateComputeStage->beginRecording();
    for (int a = 0; a < renderables.size(); a++) {
        if (renderables[a]->needsDataUpdate()) {
            renderables[a]->updateData(celestialDataUpdateComputeStage);
        }
    }
    celestialDataUpdateComputeStage->endRecording();
    celestialDataUpdateComputeStage->submitNoSemaphores({});
}
