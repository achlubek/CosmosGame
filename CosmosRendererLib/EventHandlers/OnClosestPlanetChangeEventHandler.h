#pragma once
class OnClosestPlanetChangeEventHandler : public IEventHandler
{
public:
    OnClosestPlanetChangeEventHandler(CosmosRenderer* cosmosRenderer);
    ~OnClosestPlanetChangeEventHandler();

    virtual std::string getSupportedName() override;
    virtual void handle(IEvent * event) override;

private:
    CosmosRenderer * cosmosRenderer;
};

