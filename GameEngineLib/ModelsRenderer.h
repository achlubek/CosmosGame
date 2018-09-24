#pragma once

class SceneProvider;

class ModelsRenderer
{
public:
    ModelsRenderer(VEngine::Renderer::VulkanToolkit* vulkan, int width, int height);
    ~ModelsRenderer();
    void draw(SceneProvider* scene);
    void setRenderingScale(double renderingScale);
    VEngine::Renderer::VulkanImage* getAlbedoRoughnessImage();
    VEngine::Renderer::VulkanImage* getEmissionImage();
    VEngine::Renderer::VulkanImage* getNormalMetalnessImage();
    VEngine::Renderer::VulkanImage* getDistanceImage();
    VEngine::Renderer::VulkanImage* getShadowDistanceImage();
    VEngine::Renderer::VulkanDescriptorSetLayout* getModelMRTLayout();
    void updateCameraBuffer(Camera * camera, glm::mat4 starLookAtThisMatrix);
private:
    VEngine::Renderer::VulkanToolkit* vulkan;
    int width;
    int height;

    VEngine::Renderer::VulkanDescriptorSetLayout* modelMRTLayout{ nullptr };
    VEngine::Renderer::VulkanRenderStage* modelsStage{ nullptr };
    VEngine::Renderer::VulkanRenderStage* modelsShadowStage{ nullptr };
    VEngine::Renderer::VulkanDescriptorSetLayout* modelsDataLayout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSet* modelsDataSet{ nullptr };
    VEngine::Renderer::VulkanGenericBuffer* modelsDataBuffer;
    VEngine::Renderer::VulkanGenericBuffer* sunlightDataBuffer;

    VEngine::Renderer::VulkanImage* modelsAlbedoRoughnessImage;
    VEngine::Renderer::VulkanImage* modelsEmissionImage;
    VEngine::Renderer::VulkanImage* modelsNormalMetalnessImage;
    VEngine::Renderer::VulkanImage* modelsDistanceImage;
    VEngine::Renderer::VulkanImage* modelsIDImage;
    VEngine::Renderer::VulkanImage* modelsDepthImage;

    VEngine::Renderer::VulkanImage* modelsShadowDistanceImage;

    double renderingScale{ 1.0 };
};

