#pragma once
class AbsGameStage;
class GameStageCollection
{
public:
    GameStageCollection();
    ~GameStageCollection();
    void addStage(std::string name, AbsGameStage* stage);
    AbsGameStage* getStage(std::string name);
private:
    std::map<std::string, AbsGameStage*> map{};
};

