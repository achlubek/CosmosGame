#include "stdafx.h"
#include "EventBus.h"


EventBus::EventBus()
{
}


EventBus::~EventBus()
{
}

void EventBus::registerHandler(IEventHandler * handler)
{
    handlers.push_back(handler);
}

void EventBus::enqueue(IEvent* event)
{
    events.emplace(event);
}

void EventBus::processQueue()
{
    while (events.size() > 0) {
        auto event = events.front();
        processSingleEvent(event);
        events.pop();
        delete event;
    }
}

void EventBus::processSingleEvent(IEvent* event)
{
    for (const auto &handler : handlers) {
        if (handler->getSupportedName() == event->getName()) {
            handler->handle(event);
        }
    }
}
