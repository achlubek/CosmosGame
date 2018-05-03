#pragma once
#include "Interpolator.h"
#include "CosmosRenderer.h"
#include "PointerDrivenCameraStrategy.h"
class CinematicScenarioReader
{
public:
    CinematicScenarioReader(Interpolator* interpolator, CosmosRenderer* cosmos, PointerDrivenCameraStrategy* camera, TimeProvider* time);
    ~CinematicScenarioReader();
    void load(std::string mediakey, double timeOffset);
    void execute();

private:
    std::vector<AbsInterpolatorTask*> tasks;
    Interpolator* interpolator;
    CosmosRenderer* cosmos;
    PointerDrivenCameraStrategy* camera;
    TimeProvider* time;
};

