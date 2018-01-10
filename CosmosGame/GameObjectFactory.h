#pragma once

enum GameObjectArcheTypes {
    Player,
    Ship
};
class AbsGameObjectBuilder;
class GameObjectFactory
{
public:
    GameObjectFactory();
    ~GameObjectFactory();

private:
    std::unordered_map<GameObjectArcheTypes, AbsGameObjectBuilder> builders;
};

