#pragma once
class OnClosestMoonChangeEventHandler : public IEventHandler
{
public:
    OnClosestMoonChangeEventHandler(CosmosRenderer* cosmosRenderer);
    ~OnClosestMoonChangeEventHandler();

    virtual std::string getSupportedName() override;
    virtual void handle(IEvent * event) override;

private:
    CosmosRenderer * cosmosRenderer;
};

