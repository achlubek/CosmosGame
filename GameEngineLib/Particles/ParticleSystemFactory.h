#pragma once
class ParticleSystem;
class ParticleSystemFactory
{
public:
    ParticleSystemFactory(VEngine::FileSystem::Media * media);
    ~ParticleSystemFactory();

    ParticleSystem* build(std::string mediakey);
private:
    std::map<std::string, ParticleSystem*> cache;
    VEngine::FileSystem::Media * media;
};

