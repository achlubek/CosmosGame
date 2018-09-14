#pragma once
#include "AbsCameraViewStrategy.h"
class PointerDrivenCameraStrategy : public AbsCameraViewStrategy
{
public:
    PointerDrivenCameraStrategy();
    ~PointerDrivenCameraStrategy();
    virtual void update(double elapsed, CameraController * controller) override;
    glm::dvec3* getPositionPointer();
    glm::dquat* getOrientationPointer();
    double* getFovPointer();
private:
    glm::dvec3 position;
    glm::dquat orientation;
    double fov;
};

