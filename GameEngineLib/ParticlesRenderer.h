#pragma once
class ParticleSystem;

class ParticlesRenderer
{
public:
    ParticlesRenderer(VulkanToolkit* vulkan, int width, int height, VulkanImage* mrtDistanceTexture);
    ~ParticlesRenderer();
    void update(double elapsed);
    void draw();
    void setRenderingScale(double renderingScale);
    VulkanDescriptorSetLayout* getParticleLayout();
    VulkanImage* getResultImage();
    void updateCameraBuffer(Camera * camera, glm::dvec3 observerPosition);
    void registerParticleSystem(ParticleSystem* system);
private:
    VulkanToolkit * vulkan;
    int width;
    int height;

    VulkanDescriptorSetLayout* particleLayout{ nullptr };
    VulkanRenderStage* renderStage{ nullptr };
    VulkanDescriptorSetLayout* rendererDataLayout{ nullptr };
    VulkanDescriptorSet* rendererDataSet{ nullptr };
    VulkanGenericBuffer* rendererDataBuffer;

    VulkanImage* mrtDistanceTexture;
    VulkanImage* resultImage;
    double renderingScale{ 1.0 };

    std::vector<ParticleSystem*> particleSystems;
};

