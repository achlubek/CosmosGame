#include "stdafx.h"
#include "Camera.h"

using namespace glm;

Camera::Camera()
{
    projectionMatrix = mat4(1);
    farPlane = 1000.0;
    fov = 90.0f;
    updateFocalLength();
    cone = new FrustumCone();
    position = glm::dvec3(0.0);
    orientation = glm::quat(1.0, 0.0, 0.0, 0.0);
}

Camera::~Camera()
{
}

void Camera::createProjectionPerspective(float ifov, float aspectRatio, float nearpl, float farpl)
{
    fov = ifov;
    updateFocalLength();
    farPlane = farpl;
    projectionMatrix = perspectiveRH(glm::radians(fov), aspectRatio, nearpl, farpl);
}

void Camera::createProjectionOrthogonal(float minx, float miny, float minz, float maxx, float maxy, float maxz)
{
    fov = 0.001;
    updateFocalLength();
    farPlane = 0.0;
    projectionMatrix = orthoRH(minx, maxx, miny, maxy, minz, maxz);
}

void Camera::updateFocalLength()
{
    focalLength = (float)(43.266f / (2.0f * tan(3.1415f * fov / 360.0f))) / 1.5f;
}

float Camera::getFarPlane()
{
    return farPlane;
}

float Camera::getFocalLength()
{
    return focalLength;
}

float Camera::getFov()
{
    return fov;
}

FrustumCone * Camera::getFrustumCone()
{
    return cone;
}

glm::mat4 Camera::getProjectionMatrix()
{
    return projectionMatrix;
}

glm::mat4 Camera::getRotationProjectionMatrix()
{
    return projectionMatrix * inverse(glm::mat4_cast(orientation));
}

glm::quat Camera::getOrientation()
{
    return orientation;
}

glm::dvec3 Camera::getPosition()
{
    return position;
}

void Camera::setOrientation(glm::quat orient)
{
    orientation = orient;
}

void Camera::setPosition(glm::dvec3 pos)
{
    position = pos;
}

void Camera::updateFrustumCone()
{
    glm::mat4 rpmatrix = projectionMatrix * inverse(glm::mat4_cast(orientation));
    cone->update(inverse(rpmatrix));
}
