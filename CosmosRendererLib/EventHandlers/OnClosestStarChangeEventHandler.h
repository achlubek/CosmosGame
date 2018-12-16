#pragma once
class OnClosestStarChangeEventHandler : public IEventHandler
{
public:
    OnClosestStarChangeEventHandler(CosmosRenderer* cosmosRenderer);
    ~OnClosestStarChangeEventHandler();

    virtual std::string getSupportedName() override;
    virtual void handle(IEvent * event) override;

private:
    CosmosRenderer * cosmosRenderer;
};

