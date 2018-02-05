#pragma once
class InvokeQueue
{
public:
    InvokeQueue();
    ~InvokeQueue();
    void enqueue(std::function<void(void)> func);
    void executeAll();
private:
    std::queue<std::function<void(void)>> invokeQueue;
};

