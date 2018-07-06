#include "stdafx.h"
#include "CameraFirstPersonStrategy.h"
#include "CameraController.h"
#include "AbsGameContainer.h"
#include "GameControls.h"
#include "GameObject.h"
#include "Transformation3DComponent.h"

CameraFirstPersonStrategy::CameraFirstPersonStrategy()
{
}


CameraFirstPersonStrategy::~CameraFirstPersonStrategy()
{
}

void CameraFirstPersonStrategy::update(double elapsed, CameraController * controller)
{
    auto physicsComponent = controller->getTarget()->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);
    auto targetPos = physicsComponent->getPosition();
    auto targetOrientation = physicsComponent->getOrientation();

    auto controls = AbsGameContainer::getInstance()->getControls();
    auto cursorpos = controls->getCursorPosition();

    glm::vec2 delta = cursorpos - lastCursorPos;

    lastCursorPos = cursorpos;

    rotx += delta.y * -0.02;
    roty += delta.x * -0.02;

    if (rotx < -PI * 0.5 + 0.01) rotx = -PI * 0.5 + 0.01;
    if (rotx > PI * 0.5 - 0.01) rotx = PI * 0.5 - 0.01;

    if (roty < -PI * 2.0) roty = -PI * 2.0;
    if (roty > PI * 2.0) roty = PI * 2.0;

    auto rotxmat = glm::angleAxis(rotx, glm::dvec3(1.0, 0.0, 0.0));
    auto rotymat = glm::angleAxis(roty, glm::dvec3(0.0, 1.0, 0.0));

    rotx *= 0.97;
    roty *= 0.97;

    controller->setPosition(targetPos);
    controller->setOrientation(targetOrientation * rotymat * rotxmat);
}
