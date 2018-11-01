#pragma once
class AbsCommand;
class AbsCommandHandler
{
public:
    AbsCommandHandler();
    virtual ~AbsCommandHandler();
    virtual std::string getSupportedName() = 0;
    virtual void handle(AbsCommand* command) = 0;
};

