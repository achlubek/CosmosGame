#include "stdafx.h"
#include "CameraController.h"
#include "AbsCameraViewStrategy.h" 
#include "AbsGameContainer.h" 
#include "ManualCameraStrategy.h" 
#include "CameraChaseStrategy.h" 


CameraController::CameraController()
    : internalCamera(new Camera()), position(glm::dvec3(0.0)), activeViewStrategy(new ManualCameraStrategy())
{
    auto resolution = AbsGameContainer::getInstance()->getResolution();
    internalCamera->createProjectionPerspective(90.0f, resolution.x / resolution.y, 0.0001f, 600.0f);
}


CameraController::~CameraController()
{
}

float CameraController::getFov()
{
    return internalCamera->fov;
}

void CameraController::setFov(float fov)
{
    auto resolution = AbsGameContainer::getInstance()->getResolution();
    internalCamera->createProjectionPerspective(fov, resolution.x / resolution.y, 0.0001f, 600.0f);
}

GameObject * CameraController::getTarget()
{
    return target;
}

void CameraController::setTarget(GameObject * obj)
{
    target = obj;
}

glm::dvec3 CameraController::getPosition()
{
    return position;
}

void CameraController::setPosition(glm::dvec3 pos)
{
    position = pos;
}

void CameraController::lookAt(glm::dvec3 point)
{
    glm::vec3 lowresRelative = glm::normalize(point - getPosition());
    internalCamera->transformation->setOrientation(glm::quat_cast(glm::lookAt(glm::vec3(0.0), lowresRelative, glm::vec3(0.0, 1.0, 0.0))));
}

void CameraController::lookAtDirection(glm::dvec3 dir)
{
    internalCamera->transformation->setOrientation(glm::lookAt(glm::vec3(0.0), glm::normalize(glm::vec3(dir)), glm::vec3(0.0, 1.0, 0.0)));
}

void CameraController::setOrientation(glm::dquat orient)
{
    internalCamera->transformation->setOrientation(orient);
}

void CameraController::update(double elapsed)
{
    activeViewStrategy->update(elapsed, this);
}

Camera * CameraController::getInternalCamera()
{
    return internalCamera;
}
void CameraController::setStrategy(AbsCameraViewStrategy * strategy)
{
    activeViewStrategy = strategy;
}