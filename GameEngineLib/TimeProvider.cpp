#include "stdafx.h"
#include "TimeProvider.h"


TimeProvider::TimeProvider()
{
}


TimeProvider::~TimeProvider()
{
}

void TimeProvider::setTime(double secondsElapsed)
{
    elapsed = secondsElapsed;
    synchronizeLastTime();
}

double TimeProvider::getTime()
{
    return elapsed;
}

double TimeProvider::getLastTime()
{
    return lastTime;
}

void TimeProvider::synchronizeLastTime()
{
    lastTime = elapsed;
}

void TimeProvider::update(double secondsElapsed)
{
    elapsed += secondsElapsed;
}
