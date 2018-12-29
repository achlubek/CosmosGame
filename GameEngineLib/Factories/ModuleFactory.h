#pragma once
class AbsComponent;
class Model3dFactory;
class ModuleFactory
{
public:
    ModuleFactory(Model3dFactory* model3dFactory, VEngine::FileSystem::MediaInterface * media);
    ~ModuleFactory();

    AbsComponent* build(std::string mediakey);
private:
    Model3dFactory * model3dFactory;
    VEngine::FileSystem::MediaInterface * media;
};

