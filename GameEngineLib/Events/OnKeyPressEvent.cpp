#include "stdafx.h"
#include "OnKeyPressEvent.h"


OnKeyPressEvent::OnKeyPressEvent(std::string key)
    : key(key)
{
}

OnKeyPressEvent::~OnKeyPressEvent()
{
}

std::string OnKeyPressEvent::getName()
{
    return "OnKeyPressEvent";
}
