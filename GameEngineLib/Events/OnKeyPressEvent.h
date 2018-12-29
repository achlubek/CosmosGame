#pragma once
class OnKeyPressEvent : public IEvent
{
public:
    OnKeyPressEvent(std::string key);
    ~OnKeyPressEvent();

    virtual std::string getName() override;

    std::string key;
};

