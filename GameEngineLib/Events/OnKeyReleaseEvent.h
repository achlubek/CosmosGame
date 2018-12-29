#pragma once
class OnKeyReleaseEvent : public IEvent
{
public:
    OnKeyReleaseEvent(std::string key);
    ~OnKeyReleaseEvent();

    virtual std::string getName() override;

    std::string key;
};

