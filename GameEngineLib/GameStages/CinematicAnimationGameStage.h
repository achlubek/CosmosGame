#pragma once
class GameContainer;
class GameObject;
class PointerDrivenCameraStrategy;
#include "AbsGameStage.h"
class CinematicAnimationGameStage : public AbsGameStage
{
public:
    CinematicAnimationGameStage(GameContainer* container, std::string mediaKey, std::string switchToStage);
    ~CinematicAnimationGameStage();
    virtual void onDraw() override;
    virtual void onSwitchTo() override;
    virtual void onSwitchFrom() override;
    virtual void onUpdate(double elapsed) override;
    virtual void onKeyDown(std::string key) override;
    virtual void onKeyUp(std::string key) override;
private:
    PointerDrivenCameraStrategy * pointerDrivenCamera;
    std::string switchToStage;
    std::string mediaKey;
    double animationDuration{ 0 };
    bool started = false;
    virtual void onUpdateObject(GameObject * object, double elapsed) override;
};