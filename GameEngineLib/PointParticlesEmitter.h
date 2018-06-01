#pragma once
#include <random>
class ParticleSystem;
class PointParticlesEmitter
{
public:
    PointParticlesEmitter(ParticleSystem* system);
    ~PointParticlesEmitter();

    void setParticlesPerSecond(float count);
    void updateProperties(glm::dvec3 position, glm::dvec3 velocity, glm::dvec3 direction, double startVelocity, double velocityRandomness, double directionRandomness);
    void update(double elapsed);
    ParticleSystem* getSystem();
private:
    ParticleSystem * system;
    glm::dvec3 position;
    glm::dvec3 velocity;
    glm::dvec3 direction;
    double startVelocity;
    double velocityRandomness;
    double directionRandomness;

    std::random_device rd;
    std::mt19937_64 eng;

    float generationTimeout;
    float generationTimeoutCounter{ 0 };
    bool ifTimeoutAllowsGeneration();
    double drandnorm();
};

