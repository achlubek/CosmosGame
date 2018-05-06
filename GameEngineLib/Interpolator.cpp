#include "stdafx.h"
#include "Interpolator.h"


Interpolator::Interpolator()
{
}


Interpolator::~Interpolator()
{
}

void Interpolator::addDoubleInterpolator(double* valuePointer, double target, double start, double duration, double smoothPower)
{
    auto task = new DoubleInterpolatorTask(valuePointer, target, start, duration, smoothPower);
    tasks.push_back(task);
}

void Interpolator::addVec3Interpolator(glm::dvec3* valuePointer, glm::dvec3 target, double start, double duration, double smoothPower)
{
    auto task = new Vec3InterpolatorTask(valuePointer, target, start, duration, smoothPower);
    tasks.push_back(task);
}

void Interpolator::addQuatInterpolator(glm::dquat* valuePointer, glm::dquat target, double start, double duration, double smoothPower)
{
    auto task = new QuatInterpolatorTask(valuePointer, target, start, duration, smoothPower);
    tasks.push_back(task);
}

void Interpolator::addPremadeInterpolator(AbsInterpolatorTask * task)
{
    tasks.push_back(task);
}

void Interpolator::update(double time)
{
    for (int i = 0; i < tasks.size(); i++) {
        tasks[i]->update(time);
    }
    removeFinishedInterpolators();
}

void Interpolator::removeFinishedInterpolators()
{
    for (int i = 0; i < tasks.size(); i++) {
        if (tasks[i]->hasFinished() == true) {
            delete tasks[i];
            tasks[i] = tasks[tasks.size() - 1];
            tasks.pop_back();
            --i;
        }
    }
}

void Interpolator::removeAllByTag(std::string tag)
{
    for (int i = 0; i < tasks.size(); i++) {
        if (tasks[i]->getTag() == tag) {
            delete tasks[i];
            tasks[i] = tasks[tasks.size() - 1];
            tasks.pop_back();
            --i;
        }
    }
}