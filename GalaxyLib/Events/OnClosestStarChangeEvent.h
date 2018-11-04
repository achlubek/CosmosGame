#pragma once
#include "../Star.h"
class OnClosestStarChangeEvent : public AbsEvent
{
public:
    OnClosestStarChangeEvent(Star star);
    ~OnClosestStarChangeEvent();

    virtual std::string getName() override;

    Star star;
};

