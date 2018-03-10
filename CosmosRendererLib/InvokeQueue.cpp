#include "stdafx.h"
#include "InvokeQueue.h"


InvokeQueue::InvokeQueue()
    : invokeQueue(std::queue<std::function<void(void)>>())
{
}


InvokeQueue::~InvokeQueue()
{
}

void InvokeQueue::enqueue(std::function<void(void)> func)
{
    invokeQueue.push(func);
}

void InvokeQueue::executeAll()
{
    size_t c = invokeQueue.size();
    for (size_t i = 0; i < c; i++) {
        auto func = invokeQueue.front();
        func();
        invokeQueue.pop();
    }
}
