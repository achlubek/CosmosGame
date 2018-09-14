#include "stdafx.h"
#include "ModuleFactory.h"


ModuleFactory::ModuleFactory(Model3dFactory* model3dFactory, ParticleSystemFactory* particleSystemFactory, Media * media)
    : model3dFactory(model3dFactory),
    particleSystemFactory(particleSystemFactory),
    media(media)
{
}


ModuleFactory::~ModuleFactory()
{
}

bool ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

AbsComponent * ModuleFactory::build(std::string mediakey)
{
    INIReader reader = INIReader(media, mediakey);
     
    if (ends_with(mediakey, ".engine.ini")) {
        auto particleSystem = particleSystemFactory->build(reader.gets("particle_system"));
        return new ThrustGeneratorComponent(model3dFactory->build(reader.gets("model3d")), reader.gets("model3d"),
            glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0),
            reader.getf("power"), reader.getf("wattage"), particleSystem, reader.gets("particle_system"));
    }
}
