#pragma once
class AbsGameContainer;
class GameObject;
class PointerDrivenCameraStrategy;
#include "AbsGameStage.h"
class CinematicAnimationGameStage : public AbsGameStage
{
public:
    CinematicAnimationGameStage(AbsGameContainer* container, std::string mediaKey, std::string switchToStage);
    ~CinematicAnimationGameStage();
    virtual void onDraw() override;
    virtual void onSwitchTo() override;
    virtual void onSwitchFrom() override;
    virtual void onUpdate(double elapsed) override;
private:
    PointerDrivenCameraStrategy * pointerDrivenCamera;
    std::string switchToStage;
    std::string mediaKey;
    UIText* freeFlightButton;
    UIText* editorButton;
    UIText* quitButton;
    double animationDuration{ 0 };
    bool started = false;
    virtual void onUpdateObject(GameObject * object, double elapsed) override;
};