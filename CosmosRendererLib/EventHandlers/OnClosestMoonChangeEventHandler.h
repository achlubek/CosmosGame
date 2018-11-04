#pragma once
class OnClosestMoonChangeEventHandler : public AbsEventHandler
{
public:
    OnClosestMoonChangeEventHandler(CosmosRenderer* cosmosRenderer);
    ~OnClosestMoonChangeEventHandler();

    virtual std::string getSupportedName() override;
    virtual void handle(AbsEvent * event) override;

private:
    CosmosRenderer * cosmosRenderer;
};

