#include "stdafx.h"
#include "ParticleSystem.h"
#include "ParticleSystemFactory.h"
#include "AbsGameContainer.h"
#include "ParticlesRenderer.h"


ParticleSystemFactory::ParticleSystemFactory()
    : cache({})
{
}


ParticleSystemFactory::~ParticleSystemFactory()
{
}

ParticleSystem * ParticleSystemFactory::build(std::string mediakey)
{
    if (cache.find(mediakey) != cache.end())
        return cache.at(mediakey);
    
    INIReader reader = INIReader(mediakey);

    auto texture = AbsGameContainer::getInstance()->getAssetLoader()->loadTextureFile(reader.gets("texture"));

    auto system = new ParticleSystem(AbsGameContainer::getInstance()->getVulkanToolkit(), texture, AbsGameContainer::getInstance()->getParticlesRenderer()->getParticleLayout(),
        reader.geti("max_count"),
        reader.getf("start_size"),
        reader.getf("start_transparency"),
        reader.getf("start_rotation_speed"),
        reader.getf("end_size"),
        reader.getf("end_transparency"),
        reader.getf("end_rotation_speed"),
        reader.getf("life_time")
    );
    AbsGameContainer::getInstance()->getParticlesRenderer()->registerParticleSystem(system);
    cache[mediakey] = system;
    return system;
}
