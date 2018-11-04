#pragma once
class OnClosestStarChangeEventHandler : public AbsEventHandler
{
public:
    OnClosestStarChangeEventHandler(CosmosRenderer* cosmosRenderer);
    ~OnClosestStarChangeEventHandler();

    virtual std::string getSupportedName() override;
    virtual void handle(AbsEvent * event) override;

private:
    CosmosRenderer * cosmosRenderer;
};

