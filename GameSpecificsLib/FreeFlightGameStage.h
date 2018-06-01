#pragma once
#include "AbsGameStage.h"
class GameContainer;
class ParticleSystem;

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
    UIText* fpsText;
    UIText* gravityFluxText;
    UIText* starNameText;
    UIText* planetNameText;
    UIText* moonNameText;
    UIText* altitudeText;
    UIText* velocityText;
};

