#include "stdafx.h"
#include "CameraOrbitStrategy.h"
#include "CameraController.h"
#include "GameContainer.h"
#include "GameControls.h"
#include "GameObject.h"
#include "Transformation3DComponent.h"

CameraOrbitStrategy::CameraOrbitStrategy()
{
}


CameraOrbitStrategy::~CameraOrbitStrategy()
{
}

void CameraOrbitStrategy::update(double elapsed, CameraController * controller)
{
    auto targetPos = controller->getTarget()->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getPosition();

    auto cursorpos = GameContainer::getInstance()->getControls()->getCursorPosition();

    glm::vec2 delta = cursorpos - lastCursorPos;

    lastCursorPos = cursorpos;

    rotx += delta.y * -0.02;
    roty += delta.x * -0.02; 

    if (rotx < -PI * 0.5 + 0.01) rotx = -PI * 0.5 + 0.01;
    if (rotx > PI * 0.5 - 0.01) rotx = PI * 0.5 - 0.01;

    if (roty < 0.0) roty = PI * 2.0;
    if (roty > PI * 2.0) roty = 0.0;

    printf("%f\n", rotx);

    auto rotxmat = glm::angleAxis(rotx, glm::dvec3(1.0, 0.0, 0.0));
    auto rotymat = glm::angleAxis(roty, glm::dvec3(0.0, 1.0, 0.0));

    auto direction = rotymat * (rotxmat * glm::dvec3(0.0, 0.0, distance));
    controller->setPosition(targetPos + direction);
    //controller->lookAtDirection(direction);
    controller->setOrientation(rotymat * rotxmat);
  //  double abstime = glfwGetTime();
 //   glm::vec2 sincos = glm::vec2(sin(abstime), cos(abstime));
//    controller->lookAt(glm::dvec3(sincos.x, 0.0, sincos.y));
}
