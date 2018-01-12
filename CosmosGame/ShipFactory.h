#pragma once
class GameObject;
class ShipFactory
{
public:
    ShipFactory();
    ~ShipFactory();

    GameObject* build(int id);
};

