#include "stdafx.h"
#include "CommandBus.h"


CommandBus::CommandBus()
{
}


CommandBus::~CommandBus()
{
}

void CommandBus::registerHandler(AbsCommandHandler * handler)
{
    if (handlers.find(handler->getSupportedName()) != handlers.end()) {
        throw "Handler for this name already registered: " + handler->getSupportedName();
    }
    else {
        handlers[handler->getSupportedName()] = handler;
    }
}

void CommandBus::enqueue(AbsCommand* command)
{
    commands.emplace(command);
}

void CommandBus::processQueue()
{
    while (commands.size() > 0) {
        auto command = commands.front();
        processSingleCommand(command);
        commands.pop();
        delete command;
    }
}

void CommandBus::processSingleCommand(AbsCommand* command)
{
    if (handlers.find(command->getName()) != handlers.end()) {
        handlers[command->getName()]->handle(command);
    }
    else {
        throw "Handler not found for command with name: " + command->getName();
    }
}
