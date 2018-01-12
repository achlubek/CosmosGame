#pragma once
#include "AbsCameraViewStrategy.h"
class CameraOrbitStrategy : public AbsCameraViewStrategy
{
public:
    CameraOrbitStrategy();
    ~CameraOrbitStrategy();

    virtual void update(double elapsed, CameraController * controller) override;
private:
    glm::ivec2 lastCursorPos;
    double rotx{ 0 }, roty{ 0 }, distance{ 10 };
};

