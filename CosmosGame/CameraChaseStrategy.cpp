#include "stdafx.h"
#include "CameraChaseStrategy.h"
#include "CameraController.h"
#include "GameContainer.h"
#include "GameControls.h"
#include "GameObject.h"
#include "Transformation3DComponent.h"


CameraChaseStrategy::CameraChaseStrategy(bool iautomaticAlignEnabled)
    : automaticAlignEnabled(iautomaticAlignEnabled)
{
}


CameraChaseStrategy::~CameraChaseStrategy()
{
}


void CameraChaseStrategy::update(double elapsed, CameraController * controller)
{
    auto targetPos = controller->getTarget()->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getPosition();
    auto targetOrient = controller->getTarget()->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getOrientation();

    auto controls = GameContainer::getInstance()->getControls();
    auto cursorpos = controls->getCursorPosition();

    glm::vec2 delta = cursorpos - lastCursorPos;

    lastCursorPos = cursorpos;

    rotx += delta.y * -0.02;
    roty += delta.x * -0.02;
    if (glm::length(delta) > 0.0) {
        autoKillRotCounter = 4.0;
    }
    autoKillRotCounter -= elapsed;
    autoKillRotCounter = max(autoKillRotCounter, 0.0);
    bool shouldKillRot = autoKillRotCounter == 0.0;

    if (rotx < -PI * 0.5 + 0.01) rotx = -PI * 0.5 + 0.01;
    if (rotx > PI * 0.5 - 0.01) rotx = PI * 0.5 - 0.01;

    if (roty < -PI * 2.0) roty = PI * 2.0;
    if (roty > PI * 2.0) roty = -PI * 2.0;

    if (shouldKillRot && automaticAlignEnabled) {
        if (roty > 0.0) roty -= elapsed;
        else if (roty < 0.0) roty += elapsed;
        if (abs(roty) <= elapsed) roty = 0.0;
        if (rotx > 0.0) rotx -= elapsed;
        else if (rotx < 0.0) rotx += elapsed;
        if (abs(rotx) <= elapsed) rotx = 0.0;
    }

    auto rotxmat = glm::angleAxis(rotx, glm::dvec3(1.0, 0.0, 0.0));
    auto rotymat = glm::angleAxis(roty, glm::dvec3(0.0, 1.0, 0.0));

    auto direction = targetOrient * (rotymat * (rotxmat * glm::dvec3(0.0, 0.0, distance)));
    controller->setPosition(targetPos + direction);
    //controller->lookAtDirection(direction);
    controller->setOrientation(targetOrient * rotymat * rotxmat);
    //  double abstime = glfwGetTime();
    //   glm::vec2 sincos = glm::vec2(sin(abstime), cos(abstime));
    //    controller->lookAt(glm::dvec3(sincos.x, 0.0, sincos.y));
    distance += controls->readAxisValue("camera_orbit_distance_axis") * 0.01;
    distance = glm::clamp(distance, 0.001, 1000.0);
}
