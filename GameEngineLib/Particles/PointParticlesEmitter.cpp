#include "stdafx.h"
#include "PointParticlesEmitter.h"


PointParticlesEmitter::PointParticlesEmitter(ParticleSystem* system)
    : system(system),
    position(glm::dvec3(0.0)),
    velocity(glm::dvec3(0.0)),
    direction(glm::dvec3(0.0, 1.0, 0.0)),
    generationTimeout(999999.0),
    startVelocity(1.0),
    velocityRandomness(1.0),
    directionRandomness(1.0),
    eng(std::mt19937_64(rd()))
{
}


PointParticlesEmitter::~PointParticlesEmitter()
{
}

void PointParticlesEmitter::setParticlesPerSecond(float count)
{
    if (count <= 0.0) count = 0.0000001;
    generationTimeout = 1.0 / count;
}

void PointParticlesEmitter::updateProperties(glm::dvec3 iposition, glm::dvec3 ivelocity, glm::dvec3 idirection, double istartVelocity, double ivelocityRandomness, double idirectionRandomness)
{
    position = iposition;
    velocity = ivelocity;
    direction = idirection;
    startVelocity = istartVelocity;
    velocityRandomness = ivelocityRandomness;
    directionRandomness = idirectionRandomness;
}

void PointParticlesEmitter::update(double elapsed)
{
    generationTimeoutCounter = glm::clamp(generationTimeoutCounter, 0.0f, generationTimeout);
    if (generationTimeoutCounter < generationTimeout) {
        generationTimeoutCounter += elapsed;
    }
    while (ifTimeoutAllowsGeneration()) {
        double rdRotation = drandnorm() * 3.1415 * 2.0;
        double rdVelocity = drandnorm() * 2.0 - 1.0;
        double rdPositionizer = drandnorm();
        glm::dvec3 rdDirection = glm::dvec3(drandnorm(), drandnorm(), drandnorm()) * 2.0 - 1.0;
        glm::dvec3 newDirection = glm::normalize(direction + rdDirection * directionRandomness);
        double newStartVelocity = max(startVelocity + rdVelocity * velocityRandomness, 0.0);
        system->generate(position + rdPositionizer * elapsed * newStartVelocity * newDirection * 0.01, velocity + newStartVelocity * newDirection, rdRotation);
    }
}

bool PointParticlesEmitter::ifTimeoutAllowsGeneration()
{
    if (generationTimeoutCounter >= generationTimeout) {
        generationTimeoutCounter -= generationTimeout;
        return true;
    }
    return false;
}

double PointParticlesEmitter::drandnorm()
{
    std::uniform_int_distribution<uint64_t> distr = std::uniform_int_distribution<uint64_t>();
    return ((double)distr(eng)) / ((double)UINT64_MAX);
}

ParticleSystem * PointParticlesEmitter::getSystem()
{
    return system;
}