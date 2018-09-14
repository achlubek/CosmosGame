#pragma once
class AbsGameContainer;
class GameObject;
#include "AbsGameStage.h"
class MainMenuGameStage : public AbsGameStage
{
public:
    MainMenuGameStage(AbsGameContainer* container);
    ~MainMenuGameStage();
    virtual void onDraw() override;
    virtual void onSwitchTo() override;
    virtual void onSwitchFrom() override;
    virtual void onUpdate(double elapsed) override;
    virtual void onKeyDown(std::string key) override;
    virtual void onKeyUp(std::string key) override;
private:
    UIText* freeFlightButton;
    UIText* editorButton;
    UIText* quitButton;
    virtual void onUpdateObject(GameObject * object, double elapsed) override;
};

