#pragma once
#include "AbsCameraViewStrategy.h"
class CameraFirstPersonStrategy : public AbsCameraViewStrategy
{
public:
    CameraFirstPersonStrategy();
    ~CameraFirstPersonStrategy();

    virtual void update(double elapsed, CameraController * controller) override;
private:
    glm::ivec2 lastCursorPos;
    double rotx{ 0 }, roty{ 0 };
};
