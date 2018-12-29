#pragma once
class Model3dFactory;
class ModuleFactory;
class GameObject;
class ShipFactory
{
public:
    ShipFactory(Model3dFactory* model3dFactory, ModuleFactory* moduleFactory, VEngine::FileSystem::MediaInterface* media);
    ~ShipFactory();

    GameObject* build(std::string mediakey);
private:
    VEngine::FileSystem::MediaInterface* media;
    Model3dFactory * model3dFactory;
    ModuleFactory* moduleFactory;
};

