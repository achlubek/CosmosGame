#pragma once
class GameObject;
class PlayerFactory
{
public:
    PlayerFactory();
    ~PlayerFactory();

    GameObject* build();
};

