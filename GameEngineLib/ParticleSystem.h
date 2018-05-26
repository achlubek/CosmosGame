#pragma once

struct SingleParticle
{
public:
    glm::dvec3 position;
    glm::dvec3 velocity;
    float size;
    float timeLeft;
    float rotation;
    float transparency;
};

class ParticleSystem
{
public:
    ParticleSystem(VulkanToolkit* vulkan, VulkanImage* texture, VulkanDescriptorSetLayout* particleLayout, int maxParticlesCount,
        float startSize, float startTransparency, float startRotationSpeed, float startVelocity,
        float endSize, float endTransparency, float endRotationSpeed, float endVelocity,
        float lifeTime, float generationTimeout);
    ~ParticleSystem();
    void update(double elapsed);
    void updateBuffers(glm::dvec3 observerPosition);
    void generate(glm::dvec3 position, glm::dvec3 direction);
    VulkanDescriptorSet* getSet();
    int getCount();
    void setGenerationTimeout(float timeout);
    bool ifTimeoutAllowsGeneration();
private:
    VulkanImage * texture;
    VulkanDescriptorSet * set{ nullptr };
    VulkanGenericBuffer* dataBuffer{ nullptr };

    float generationTimeout;
    float generationTimeoutCounter{ 0 };

    float startSize;
    float startTransparency;
    float startRotationSpeed;
    float startVelocity;

    float endSize;
    float endTransparency;
    float endRotationSpeed;
    float endVelocity;

    float lifeTime;

    std::vector<SingleParticle> particles;
};

