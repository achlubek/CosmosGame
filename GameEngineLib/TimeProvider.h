#pragma once
class TimeProvider
{
public:
    TimeProvider();
    ~TimeProvider();
    void setTime(double secondsElapsed);
    double getTime();
    void update(double secondsElapsed);
private:
    double elapsed{ 0 };
};

