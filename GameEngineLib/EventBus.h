#pragma once
class AbsEvent;
class AbsEventHandler;

class EventBus
{
public:
    EventBus();
    ~EventBus();
    void registerHandler(AbsEventHandler* handler);
    void enqueue(AbsEvent* event);
    void processQueue();
private:
    void processSingleEvent(AbsEvent* event);
    std::vector<AbsEventHandler*> handlers = {};
    std::queue<AbsEvent*> events = {};
};

