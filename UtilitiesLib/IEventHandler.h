#pragma once
class AbsEvent;
class IEventHandler
{
public:
    virtual std::string getSupportedName() = 0;
    virtual void handle(IEvent* event) = 0;
};

