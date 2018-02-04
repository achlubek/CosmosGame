#pragma once
#include "AbsCameraViewStrategy.h"
class CameraChaseStrategy : public AbsCameraViewStrategy
{
public:
    CameraChaseStrategy();
    ~CameraChaseStrategy();

    virtual void update(double elapsed, CameraController * controller) override;
private:
    glm::ivec2 lastCursorPos;
    double rotx{ 0 }, roty{ 0 }, distance{ 20 };
    double autoKillRotCounter{ 0 };
};
