#include "stdafx.h"
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(VulkanToolkit* vulkan, VulkanImage* texture, VulkanDescriptorSetLayout* particleLayout, int maxParticlesCount,
    float startSize, float startTransparency, float startRotationSpeed, float startVelocity,
    float endSize, float endTransparency, float endRotationSpeed, float endVelocity,
    float lifeTime)
    :  texture(texture),
    startSize(startSize),
    startTransparency(startTransparency),
    startRotationSpeed(startRotationSpeed),
    startVelocity(startVelocity),
    endSize(endSize),
    endTransparency(endTransparency),
    endRotationSpeed(endRotationSpeed),
    endVelocity(endVelocity),
    lifeTime(lifeTime),
    particles({})
{
    dataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    set = particleLayout->generateDescriptorSet();
    set->bindStorageBuffer(0, dataBuffer);
    set->bindImageViewSampler(1, texture);
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::update(double elapsed)
{
    for (int i = 0; i < particles.size(); i++) {
        particles[i].position += particles[i].velocity;
        float mixLifeTime = 1.0 - particles[i].timeLeft / lifeTime;
       // particles[i].velocity = glm::normalize(particles[i].velocity) * static_cast<double>(glm::mix(startVelocity, endVelocity, mixLifeTime));
        particles[i].size = glm::mix(startSize, endSize, mixLifeTime);
        particles[i].rotation += glm::mix(startRotationSpeed, endRotationSpeed, mixLifeTime);
        particles[i].timeLeft -= elapsed;
    }
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i].timeLeft <= 0.0) {
            particles[i] = particles[particles.size() - 1];
            particles.pop_back();
            --i;
        }
    }
}

void ParticleSystem::updateBuffers()
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();

    bb.emplaceInt32(particles.size());
    bb.emplaceInt32(particles.size());
    bb.emplaceInt32(particles.size());
    bb.emplaceInt32(particles.size());

    for (int i = 0; i < particles.size(); i++) {
        bb.emplaceFloat32(particles[i].position.x);
        bb.emplaceFloat32(particles[i].position.y);
        bb.emplaceFloat32(particles[i].position.z);
        bb.emplaceFloat32(particles[i].size);

        bb.emplaceFloat32(particles[i].rotation);
        bb.emplaceFloat32(particles[i].rotation);
        bb.emplaceFloat32(particles[i].rotation);
        bb.emplaceFloat32(particles[i].rotation);
    }    
    void* data;
    dataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    dataBuffer->unmap();
}

void ParticleSystem::generate(glm::dvec3 position, glm::dvec3 direction)
{
    SingleParticle particle = {};
    particle.position = position;
    particle.velocity = static_cast<double>(startVelocity) * direction;
    particle.size = startSize;
    particle.timeLeft = lifeTime;
    particle.rotation = 0.0f;
}
