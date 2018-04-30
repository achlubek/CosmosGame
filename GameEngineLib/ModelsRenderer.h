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
    unsigned int pickComponentId(glm::vec2 uv);
private:
    VulkanToolkit* vulkan;
    int width;
    int height;

    VulkanDescriptorSetLayout* modelMRTLayout{ nullptr };
    VulkanRenderStage* modelsStage{ nullptr };
    VulkanDescriptorSetLayout* modelsDataLayout{ nullptr };
    VulkanDescriptorSet* modelsDataSet{ nullptr };
    VulkanGenericBuffer* modelsDataBuffer;

    VulkanComputeStage* pickerStage{ nullptr };
    VulkanDescriptorSetLayout* pickerDataLayout{ nullptr };
    VulkanDescriptorSet* pickerDataSet{ nullptr };
    VulkanGenericBuffer* pickerDataBuffer;

    VulkanImage* modelsAlbedoRoughnessImage;
    VulkanImage* modelsNormalMetalnessImage;
    VulkanImage* modelsDistanceImage;
    VulkanImage* modelsIDImage;
    VulkanImage* modelsDepthImage;
    double renderingScale{ 1.0 };
};

