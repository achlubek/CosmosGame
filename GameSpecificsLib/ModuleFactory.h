#pragma once
class AbsComponent;
class Model3dFactory;
class ParticleSystemFactory;
class ModuleFactory
{
public:
    ModuleFactory(Model3dFactory* model3dFactory, ParticleSystemFactory* particleSystemFactory);
    ~ModuleFactory();

    AbsComponent* build(std::string mediakey);
private:
    Model3dFactory * model3dFactory;
    ParticleSystemFactory* particleSystemFactory;
};

