#include "stdafx.h"
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem(VulkanToolkit* vulkan, VulkanImage* texture, 
    VulkanDescriptorSetLayout* particleLayout, int maxParticlesCount,
    float startSize, float startTransparency, float startRotationSpeed, float startVelocity,
    float endSize, float endTransparency, float endRotationSpeed, float endVelocity,
    float lifeTime, float generationTimeout)
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
    generationTimeout(generationTimeout),
    particles({})
{
    dataBuffer = new VulkanGenericBuffer(vulkan, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, sizeof(float) * 1024 * 1024);
    set = particleLayout->generateDescriptorSet();
    set->bindStorageBuffer(0, dataBuffer);
    set->bindImageViewSampler(1, texture);
    set->update();
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::update(double elapsed)
{
    for (int i = 0; i < particles.size(); i++) {
        particles[i].position += particles[i].velocity * elapsed * 0.001;
        float mixLifeTime = 1.0 - particles[i].timeLeft / lifeTime;
       // particles[i].velocity = glm::normalize(particles[i].velocity) * static_cast<double>(glm::mix(startVelocity, endVelocity, mixLifeTime));
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
    generationTimeoutCounter += elapsed;
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

void ParticleSystem::generate(glm::dvec3 position, glm::dvec3 direction)
{
    SingleParticle particle = {};
    particle.position = position;
    particle.velocity = direction;
    particle.size = startSize;
    particle.timeLeft = lifeTime;
    particle.rotation = 0.0f;
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

void ParticleSystem::setGenerationTimeout(float timeout)
{
    generationTimeout = timeout;
}

bool ParticleSystem::ifTimeoutAllowsGeneration()
{
    if (generationTimeoutCounter > generationTimeout) {
        generationTimeoutCounter = 0;
        return true;
    }
    return false;
}