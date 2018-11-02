#pragma once
class AbsCommand
{
public:
    AbsCommand();
    virtual ~AbsCommand();
    virtual std::string getName() = 0;
};

