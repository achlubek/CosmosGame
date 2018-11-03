#pragma once
class TimeProvider
{
public:
    TimeProvider();
    ~TimeProvider();
    void setTime(double secondsElapsed);
    double getTime();
    double getLastTime();
    void synchronizeLastTime();
    void update(double secondsElapsed);
private:
    double elapsed{ 0 };
    double lastTime = 0.0;
};

