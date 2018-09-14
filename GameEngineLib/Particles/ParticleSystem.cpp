#include "stdafx.h"
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(VulkanToolkit* vulkan, VulkanImage* texture, 
    VulkanDescriptorSetLayout* particleLayout, int maxParticlesCount,
    float startSize, float startTransparency, float startRotationSpeed,
    float endSize, float endTransparency, float endRotationSpeed,
    float lifeTime)
    :  texture(texture),
    startSize(startSize),
    startTransparency(startTransparency),
    startRotationSpeed(startRotationSpeed),
    endSize(endSize),
    endTransparency(endTransparency),
    endRotationSpeed(endRotationSpeed),
    lifeTime(lifeTime),
    particles({})
{
    dataBuffer = vulkan->getVulkanBufferFactory()->build(VulkanBufferType::BufferTypeStorage, sizeof(float) * 1024 * 1024);
    set = particleLayout->generateDescriptorSet();
    set->bindBuffer(0, dataBuffer);
    set->bindImageViewSampler(1, texture);
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::update(double elapsed)
{
    for (int i = 0; i < particles.size(); i++) {
        particles[i].position += particles[i].velocity * elapsed * 0.01;
        float mixLifeTime = 1.0 - particles[i].timeLeft / lifeTime;
        particles[i].size = glm::mix(startSize, endSize, mixLifeTime);
        particles[i].transparency = glm::mix(startTransparency, endTransparency, mixLifeTime);
        particles[i].rotation += glm::mix(startRotationSpeed, endRotationSpeed, mixLifeTime) * elapsed;
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

void ParticleSystem::updateBuffers(glm::dvec3 observerPosition)
{
    VulkanBinaryBufferBuilder bb = VulkanBinaryBufferBuilder();

    bb.emplaceInt32(particles.size());
    bb.emplaceInt32(particles.size());
    bb.emplaceInt32(particles.size());
    bb.emplaceInt32(particles.size());

    for (int i = 0; i < particles.size(); i++) {
        auto position = (particles[i].position - observerPosition) * 0.01;
        bb.emplaceFloat32(position.x);
        bb.emplaceFloat32(position.y);
        bb.emplaceFloat32(position.z);
        bb.emplaceFloat32(particles[i].size);

        bb.emplaceFloat32(particles[i].rotation);
        bb.emplaceFloat32(particles[i].transparency);
        bb.emplaceFloat32(particles[i].rotation);
        bb.emplaceFloat32(particles[i].rotation);
    }    
    void* data;
    dataBuffer->map(0, bb.buffer.size(), &data);
    memcpy(data, bb.getPointer(), bb.buffer.size());
    dataBuffer->unmap();
}

void ParticleSystem::generate(glm::dvec3 position, glm::dvec3 velocity, float rotation)
{
    SingleParticle particle = {};
    particle.position = position;
    particle.velocity = velocity;
    particle.size = startSize;
    particle.timeLeft = lifeTime;
    particle.rotation = rotation;
    particle.transparency = startTransparency;
    particles.push_back(particle);
}

VulkanDescriptorSet * ParticleSystem::getSet()
{
    return set;
}

int ParticleSystem::getCount()
{
    return particles.size();
}
