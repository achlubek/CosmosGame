#pragma once

struct SingleParticle
{
public:
    glm::dvec3 position;
    glm::dvec3 velocity;
    float size;
    float timeLeft;
    float rotation;
};

class ParticleSystem
{
public:
    ParticleSystem(VulkanToolkit* vulkan, VulkanImage* texture, VulkanDescriptorSetLayout* particleLayout, int maxParticlesCount,
        float startSize, float startTransparency, float startRotationSpeed, float startVelocity,
        float endSize, float endTransparency, float endRotationSpeed, float endVelocity,
        float lifeTime);
    ~ParticleSystem();
    void update(double elapsed);
    void updateBuffers();
    void generate(glm::dvec3 position, glm::dvec3 direction);
private:
    VulkanImage * texture;
    VulkanDescriptorSet * set{ nullptr };
    VulkanGenericBuffer* dataBuffer{ nullptr };

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

