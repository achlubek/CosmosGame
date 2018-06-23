#pragma once
#include "AbsGameStage.h"
class GameContainer;
class ModelPreviewGameStage : public AbsGameStage
{
public:
    ModelPreviewGameStage(AbsGameContainer* container);
    ~ModelPreviewGameStage();
    virtual void onDraw() override;
    virtual void onSwitchTo() override;
    virtual void onSwitchFrom() override;
    virtual void onUpdate(double elapsed) override;
    void addPreviewObject(GameObject* object);
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
    glm::dvec3 viewCenter;
};
