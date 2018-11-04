#pragma once
class OnClosestPlanetChangeEventHandler : public AbsEventHandler
{
public:
    OnClosestPlanetChangeEventHandler(CosmosRenderer* cosmosRenderer);
    ~OnClosestPlanetChangeEventHandler();

    virtual std::string getSupportedName() override;
    virtual void handle(AbsEvent * event) override;

private:
    CosmosRenderer * cosmosRenderer;
};

