#pragma once
class GalaxyContainer;
class StarsRenderer
{
public:
    StarsRenderer(ToolkitInterface* toolkit, 
        int width, int height, double scale,
        DescriptorSetLayoutInterface* rendererDataSetLayout,
        DescriptorSetInterface* rendererDataSet,
        GalaxyContainer* galaxy);
    ~StarsRenderer();
    void draw(std::vector<SemaphoreInterface*> waitSemaphores);
    void recompile();
    ImageInterface* getStarsImage();
    SemaphoreInterface* getSignalSemaphore();
private:
    ToolkitInterface * toolkit;
    GalaxyContainer* galaxy;
    DescriptorSetLayoutInterface* rendererDataSetLayout;
    DescriptorSetInterface* rendererDataSet;

    int width;
    int height;
    double scale;
    bool doesNeedRecording;

    DescriptorSetLayoutInterface* starsDataLayout{ nullptr };
    DescriptorSetInterface* starsDataSet{ nullptr };
    RenderStageInterface* starsStage{ nullptr };
    ImageInterface* starsImage;
    GenericBufferInterface* starsDataBuffer;
    Object3dInfoInterface* cube3dInfo;

    void updateStarsBuffer();
    void createRenderStage();
};

