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
}

double TimeProvider::getTime()
{
    return elapsed;
}

void TimeProvider::update(double secondsElapsed)
{
    elapsed += secondsElapsed;
}
