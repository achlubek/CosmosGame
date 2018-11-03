#pragma once
class Model3dFactory;
class ModuleFactory;
class GameObject;
class ShipFactory
{
public:
    ShipFactory(Model3dFactory* model3dFactory, ModuleFactory* moduleFactory, VEngine::FileSystem::Media* media);
    ~ShipFactory();

    GameObject* build(std::string mediakey);
private:
    VEngine::FileSystem::Media* media;
    Model3dFactory * model3dFactory;
    ModuleFactory* moduleFactory;
};

