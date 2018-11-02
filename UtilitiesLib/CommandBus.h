#pragma once
class AbsCommand;
class AbsCommandHandler;

class CommandBus
{
public:
    CommandBus();
    ~CommandBus();
    void registerHandler(AbsCommandHandler* handler);
    void enqueue(AbsCommand* command);
    void processQueue();
private:
    void processSingleCommand(AbsCommand* command);
    std::unordered_map<std::string, AbsCommandHandler*> handlers = {};
    std::queue<AbsCommand*> commands = {};
};

