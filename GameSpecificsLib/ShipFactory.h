#pragma once
class Model3dFactory;
class ModuleFactory;
class GameObject;
class ShipFactory
{
public:
    ShipFactory(Model3dFactory* model3dFactory, ModuleFactory* moduleFactory);
    ~ShipFactory();

    GameObject* build(std::string mediakey);
private:
    Model3dFactory * model3dFactory;
    ModuleFactory* moduleFactory;
};

