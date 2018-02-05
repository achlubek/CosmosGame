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
    int c = invokeQueue.size();
    for (int i = 0; i < c; i++) {
        auto func = invokeQueue.front();
        func();
        invokeQueue.pop();
    }
}
