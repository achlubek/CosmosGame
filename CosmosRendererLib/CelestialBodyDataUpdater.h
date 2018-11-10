#pragma once
class CelestialBodyDataUpdater
{
public:
    CelestialBodyDataUpdater(VulkanToolkit* vulkan);
    ~CelestialBodyDataUpdater();
    VulkanDescriptorSetLayout * getBodyDataSetLayout();
    void update(std::vector<RenderedCelestialBody*> renderables);

private:
    VulkanDescriptorSetLayout * celestialBodyDataSetLayout{ nullptr };
    VulkanComputeStage* celestialDataUpdateComputeStage;
};

