#include "stdafx.h"
#include "OnKeyReleaseEvent.h"


OnKeyReleaseEvent::OnKeyReleaseEvent(std::string key)
    : key(key)
{
}

OnKeyReleaseEvent::~OnKeyReleaseEvent()
{
}

std::string OnKeyReleaseEvent::getName()
{
    return "OnKeyReleaseEvent";
}
