#pragma once
#include "AbsGameStage.h"
#include "GeneratedStarInfo.h"
#include "CelestialBody.h"
class AbsCelestialObject;
class GameContainer;
class CelestialBodyPreviewGameStage : public AbsGameStage
{
public:
    CelestialBodyPreviewGameStage(AbsGameContainer* container, int64_t targetStar, int64_t targetPlanet, int64_t targetMoon);
    ~CelestialBodyPreviewGameStage();
private:
    AbsCelestialObject * targetBody;
    CelestialBody celestialTarget;
    GeneratedStarInfo star;
    glm::dvec3 center = glm::dvec3(0.0);
    double dist = 0.0;
    float fov = 90.0f;
    float mindist = 100.0;

    double xrot = 0.0;
    double yrot = 0.0;
    double zrot = 0.0;

    double Rxrot = 0.0;
    double Ryrot = 0.0;
    double Rzrot = 0.0;

    int64_t targetStar = 0;
    int64_t targetPlanet = 0;
    int64_t targetMoon = 0;

    double timeScale = 0.1;

    bool disableThings = false;

    GameContainer * getCosmosGameContainer();
    virtual void onSwitchTo() override;
    virtual void onSwitchFrom() override;
    virtual void onDraw() override;
    virtual void onUpdate(double elapsed) override;
    virtual void onUpdateObject(GameObject * object, double elapsed) override;
};

