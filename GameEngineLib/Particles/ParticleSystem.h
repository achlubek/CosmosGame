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
        float startSize, float startTransparency, float startRotationSpeed,
        float endSize, float endTransparency, float endRotationSpeed,
        float lifeTime);
    ~ParticleSystem();
    void update(double elapsed);
    void updateBuffers(glm::dvec3 observerPosition);
    void generate(glm::dvec3 position, glm::dvec3 velocity, float rotation);
    VulkanDescriptorSet* getSet();
    int getCount();
private:
    VulkanImage * texture;
    VulkanDescriptorSet * set{ nullptr };
    VulkanGenericBuffer* dataBuffer{ nullptr };

    float startSize;
    float startTransparency;
    float startRotationSpeed;

    float endSize;
    float endTransparency;
    float endRotationSpeed;

    float lifeTime;

    std::vector<SingleParticle> particles;
};

