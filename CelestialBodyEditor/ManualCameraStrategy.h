#pragma once
#include "AbsCameraViewStrategy.h"
class ManualCameraStrategy : public AbsCameraViewStrategy
{
public:
    ManualCameraStrategy();
    ~ManualCameraStrategy();
    virtual void update(double elapsed, CameraController * controller) override;
};

