#pragma once
class Interpolator;
class AbsInterpolatorTask;
class CosmosRenderer;
class PointerDrivenCameraStrategy;
class TimeProvider;
class CinematicScenarioReader
{
public:
    CinematicScenarioReader(Interpolator* interpolator, CosmosRenderer* cosmos, VEngine::FileSystem::Media* media, PointerDrivenCameraStrategy* camera, TimeProvider* time);
    ~CinematicScenarioReader();
    void load(std::string mediakey, double timeOffset);
    void execute();
    double getAnimationDuration();

private:
    std::vector<AbsInterpolatorTask*> tasks;
    Interpolator* interpolator;
    CosmosRenderer* cosmos;
    VEngine::FileSystem::Media* media;
    PointerDrivenCameraStrategy* camera;
    TimeProvider* time;
    double animationDuration{ 0 };
};

