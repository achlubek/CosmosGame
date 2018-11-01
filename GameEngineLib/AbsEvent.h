#pragma once
class AbsEvent
{
public:
    AbsEvent();
    virtual ~AbsEvent();
    virtual std::string getName() = 0;
};

