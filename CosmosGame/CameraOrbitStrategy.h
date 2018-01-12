#pragma once
#include "AbsCameraViewStrategy.h"
class CameraOrbitStrategy : public AbsCameraViewStrategy
{
public:
    CameraOrbitStrategy();
    ~CameraOrbitStrategy();

    virtual void update(double elapsed, CameraController * controller) override;
};

