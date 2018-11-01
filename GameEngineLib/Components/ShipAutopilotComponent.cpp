#include "stdafx.h"
#include "ShipAutopilotComponent.h"


ShipAutopilotComponent::ShipAutopilotComponent()
    : AbsComponent(ComponentTypes::ShipAutopilot)
{
    shipOrientationControllerState = new ShipOrientationControllerState();
    orientationPitchContoller = PercentageDerivativeController(0.835, 1.73, 0.0);
    orientationYawContoller = PercentageDerivativeController(0.835, 1.73, 0.0);
}


ShipAutopilotComponent::~ShipAutopilotComponent()
{
    safedelete(shipOrientationControllerState);
}

void ShipAutopilotComponent::update(double elapsed)
{
    auto state = shipOrientationControllerState->getCurrentState();
    auto controls = GameContainer::getInstance()->getControls();
    if (controls->isKeyDown("autopilot_orientation_disabled")) {
        shipOrientationControllerState->changeState("disabled");
    }
    if (controls->isKeyDown("autopilot_orientation_stable")) {
        shipOrientationControllerState->changeState("stable");
    }
    if (controls->isKeyDown("autopilot_orientation_retrograde")) {
        shipOrientationControllerState->changeState("retrograde");
    }
    if (controls->isKeyDown("autopilot_orientation_prograde")) {
        shipOrientationControllerState->changeState("prograde");
    }
    if (state == "disabled") return;

    auto thrustController = owner->getComponent<ThrustControllerComponent>(ComponentTypes::ThrustController);
    auto physics = owner->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);

    if (state == "stable") {
        thrustController->setAngularThrust(glm::dvec3(
            glm::clamp(physics->getAngularVelocity(), -1.0, 1.0)
        ));
    }
    else {
        auto desiredDirection = glm::normalize(physics->getLinearVelocity());
        if (state == "prograde") {
            desiredDirection = glm::normalize(physics->getLinearVelocity());
        }
        if (state == "retrograde") {
            desiredDirection = -glm::normalize(physics->getLinearVelocity());
        }
        auto rotmat = glm::mat3_cast(physics->getOrientation());

        glm::dvec3 shipforward = rotmat * glm::dvec3(0.0, 0.0, -1.0);
        glm::dvec3 shipleft = rotmat * glm::dvec3(1.0, 0.0, 0.0);
        glm::dvec3 shipup = rotmat * glm::dvec3(0.0, 1.0, 0.0);
        auto fdir = shipforward + desiredDirection * 1.1;

        double xt = -glm::dot(shipup, fdir);
        double yt = -glm::dot(shipleft, fdir);

        auto pitch = glm::clamp(orientationPitchContoller.calculate(0.0, xt, elapsed), -1.0, 1.0);
        auto yaw = glm::clamp(orientationYawContoller.calculate(0.0, yt, elapsed), -1.0, 1.0);
        thrustController->setAngularThrust(glm::dvec3(
            pitch, -yaw, 0.0
        ));
    }


}

AbsComponent * ShipAutopilotComponent::clone()
{
    return new ShipManualControlsComponent();
}

std::string ShipAutopilotComponent::serialize()
{
    std::stringstream s;
    s << serializeBase();
    return s.str();
}

ShipAutopilotComponent * ShipAutopilotComponent::deserialize(std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    auto component = new ShipAutopilotComponent();
    component->deserializeBaseInPlace(serializedString);
    return component;
}
