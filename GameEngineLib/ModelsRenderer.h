#pragma once
class SceneProvider;
class ModelsRenderer
{
public:
    ModelsRenderer(VulkanToolkit* vulkan, int width, int height);
    ~ModelsRenderer();
    void draw(SceneProvider* scene);
    void setRenderingScale(double renderingScale);
    VulkanImage* getAlbedoRoughnessImage();
    VulkanImage* getNormalMetalnessImage();
    VulkanImage* getDistanceImage();
    VulkanDescriptorSetLayout* getModelMRTLayout();
    void updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition);
private:
    VulkanToolkit* vulkan;
    int width;
    int height;
    VulkanDescriptorSetLayout* modelMRTLayout{ nullptr };
    VulkanRenderStage* modelsStage{ nullptr };
    VulkanDescriptorSetLayout* modelsDataLayout{ nullptr };
    VulkanDescriptorSet* modelsDataSet{ nullptr };
    VulkanGenericBuffer* modelsDataBuffer;
    VulkanImage* modelsAlbedoRoughnessImage;
    VulkanImage* modelsNormalMetalnessImage;
    VulkanImage* modelsDistanceImage;
    VulkanImage* modelsDepthImage;
    double renderingScale{ 1.0 };
};

