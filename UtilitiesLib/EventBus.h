#pragma once
class IEvent;
class IEventHandler;

class EventBus
{
public:
    EventBus();
    ~EventBus();
    void registerHandler(IEventHandler* handler);
    void enqueue(IEvent* event);
    void processQueue();
private:
    void processSingleEvent(IEvent* event);
    std::vector<IEventHandler*> handlers = {};
    std::queue<IEvent*> events = {};
};

