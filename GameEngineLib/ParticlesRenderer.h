#pragma once
class ParticleSystem;

class ParticlesRenderer
{
public:
    ParticlesRenderer(VEngine::Renderer::VulkanToolkit* vulkan, int width, int height, VEngine::Renderer::VulkanImage* mrtDistanceTexture);
    ~ParticlesRenderer();
    void update(double elapsed);
    void draw();
    void setRenderingScale(double renderingScale);
    VEngine::Renderer::VulkanDescriptorSetLayout* getParticleLayout();
    VEngine::Renderer::VulkanImage* getResultImage();
    void updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition);
    void registerParticleSystem(ParticleSystem* system);
private:
    VEngine::Renderer::VulkanToolkit * vulkan;
    int width;
    int height;

    VEngine::Renderer::VulkanDescriptorSetLayout* particleLayout{ nullptr };
    VEngine::Renderer::VulkanRenderStage* renderStage{ nullptr };
    VEngine::Renderer::VulkanDescriptorSetLayout* rendererDataLayout{ nullptr };
    VEngine::Renderer::VulkanDescriptorSet* rendererDataSet{ nullptr };
    VEngine::Renderer::VulkanGenericBuffer* rendererDataBuffer;

    VEngine::Renderer::VulkanImage* mrtDistanceTexture;
    VEngine::Renderer::VulkanImage* resultImage;
    double renderingScale{ 1.0 };

    std::vector<ParticleSystem*> particleSystems;
};

