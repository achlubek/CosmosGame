#pragma once
#include "AbsGameStage.h"
class GameContainer;

class FreeFlightGameStage : public AbsGameStage
{
public:
    FreeFlightGameStage(AbsGameContainer* container);
    ~FreeFlightGameStage();
    virtual void onDraw() override;
    virtual void onSwitchTo() override;
    virtual void onSwitchFrom() override;
    virtual void onUpdate(double elapsed) override;
private:
    GameContainer * getCosmosGameContainer();
    virtual void onUpdateObject(GameObject* object, double elapsed) override;
    VEngine::UserInterface::UIText* fpsText;
    VEngine::UserInterface::UIText* gravityFluxText;
    VEngine::UserInterface::UIText* starNameText;
    VEngine::UserInterface::UIText* planetNameText;
    VEngine::UserInterface::UIText* moonNameText;
    VEngine::UserInterface::UIText* altitudeText;
    VEngine::UserInterface::UIText* velocityText;
};

