#pragma once
class AbsEvent;
class AbsEventHandler
{
public:
    AbsEventHandler();
    virtual ~AbsEventHandler();
    virtual std::string getSupportedName() = 0;
    virtual void handle(AbsEvent* command) = 0;
};

