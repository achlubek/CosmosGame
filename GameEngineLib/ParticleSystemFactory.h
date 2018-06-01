#pragma once
class ParticleSystem;
class ParticleSystemFactory
{
public:
    ParticleSystemFactory();
    ~ParticleSystemFactory();

    ParticleSystem* build(std::string mediakey);
private:
    std::map<std::string, ParticleSystem*> cache;
};

