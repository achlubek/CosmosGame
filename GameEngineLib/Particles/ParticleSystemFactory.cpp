#include "stdafx.h"
#include "ParticleSystem.h"


ParticleSystemFactory::ParticleSystemFactory(Media * media)
    : cache({}), media(media)
{
}


ParticleSystemFactory::~ParticleSystemFactory()
{
}

ParticleSystem * ParticleSystemFactory::build(std::string mediakey)
{
    if (cache.find(mediakey) != cache.end())
        return cache.at(mediakey);
    
    INIReader reader = INIReader(media, mediakey);

    auto texture = GameContainer::getInstance()->getVulkanToolkit()->getVulkanImageFactory()->build(reader.gets("texture"));

    auto system = new ParticleSystem(GameContainer::getInstance()->getVulkanToolkit(), texture, GameContainer::getInstance()->getParticlesRenderer()->getParticleLayout(),
        reader.geti("max_count"),
        reader.getf("start_size"),
        reader.getf("start_transparency"),
        reader.getf("start_rotation_speed"),
        reader.getf("end_size"),
        reader.getf("end_transparency"),
        reader.getf("end_rotation_speed"),
        reader.getf("life_time")
    );
    GameContainer::getInstance()->getParticlesRenderer()->registerParticleSystem(system);
    cache[mediakey] = system;
    return system;
}
