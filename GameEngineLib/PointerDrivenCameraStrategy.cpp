#include "stdafx.h"
#include "PointerDrivenCameraStrategy.h"
#include "CameraController.h"


PointerDrivenCameraStrategy::PointerDrivenCameraStrategy()
    :fov(90.0), position(glm::dvec3(0.0)), orientation(glm::dquat(1.0, 0.0, 0.0, 0.0))
{
}


PointerDrivenCameraStrategy::~PointerDrivenCameraStrategy()
{
}

void PointerDrivenCameraStrategy::update(double elapsed, CameraController * controller)
{
    controller->setFov(fov);
    controller->setPosition(position);
    controller->setOrientation(orientation);
}

glm::dvec3 * PointerDrivenCameraStrategy::getPositionPointer()
{
    return &position;
}

glm::dquat * PointerDrivenCameraStrategy::getOrientationPointer()
{
    return &orientation;
}

double * PointerDrivenCameraStrategy::getFovPointer()
{
    return &fov;
}
