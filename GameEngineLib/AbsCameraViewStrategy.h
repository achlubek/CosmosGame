#pragma once
class CameraController;
class AbsCameraViewStrategy
{
public:
    AbsCameraViewStrategy();
    virtual ~AbsCameraViewStrategy() {}
    virtual void update(double elapsed, CameraController* controller) = 0;
};

