#include "stdafx.h"
#include "CameraController.h"


CameraController::CameraController()
    : camera(new Camera()), activeViewStrategy(new ManualCameraStrategy())
{
    auto resolution = AbsGameContainer::getInstance()->getResolution();
    camera->createProjectionPerspective(90.0f, resolution.x / resolution.y, 0.00001f, 600.0f);
}


CameraController::~CameraController()
{
}

float CameraController::getFov()
{
    return camera->getFov();
}

void CameraController::setFov(float fov)
{
    auto resolution = AbsGameContainer::getInstance()->getResolution();
    camera->createProjectionPerspective(fov, resolution.x / resolution.y, 0.00001f, 600.0f);
}

GameObject * CameraController::getTarget()
{
    return target;
}

void CameraController::setTarget(GameObject * obj)
{
    target = obj;
}

void CameraController::lookAt(glm::dvec3 point)
{
    glm::vec3 lowresRelative = glm::normalize(point - camera->getPosition());
    camera->setOrientation(glm::quat_cast(glm::lookAt(glm::vec3(0.0), lowresRelative, glm::vec3(0.0, 1.0, 0.0))));
}

void CameraController::lookAtDirection(glm::dvec3 dir)
{
    camera->setOrientation(glm::lookAt(glm::vec3(0.0), glm::normalize(glm::vec3(dir)), glm::vec3(0.0, 1.0, 0.0)));
}

void CameraController::update(double elapsed)
{
    activeViewStrategy->update(elapsed, this);
}

Camera * CameraController::getCamera()
{
    return camera;
}

void CameraController::setStrategy(AbsCameraViewStrategy * strategy)
{
    activeViewStrategy = strategy;
}