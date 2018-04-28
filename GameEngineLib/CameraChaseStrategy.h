#pragma once
#include "AbsCameraViewStrategy.h"
class CameraChaseStrategy : public AbsCameraViewStrategy
{
public:
    CameraChaseStrategy(bool automaticAlignEnabled);
    ~CameraChaseStrategy();

    virtual void update(double elapsed, CameraController * controller) override;
private:
    glm::ivec2 lastCursorPos;
    double rotx{ 0 }, roty{ 0 }, distance{ 0.01 };
    double autoKillRotCounter{ 0 };
    bool automaticAlignEnabled = true;
};

