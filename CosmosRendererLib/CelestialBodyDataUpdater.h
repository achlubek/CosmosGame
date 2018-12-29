#pragma once
class CelestialBodyDataUpdater
{
public:
    CelestialBodyDataUpdater(ToolkitInterface* toolkit);
    ~CelestialBodyDataUpdater();
    DescriptorSetLayoutInterface * getBodyDataSetLayout();
    void update(std::vector<RenderedCelestialBody*> renderables);

private:
    DescriptorSetLayoutInterface * celestialBodyDataSetLayout{ nullptr };
    ComputeStageInterface* celestialDataUpdateComputeStage;
};

