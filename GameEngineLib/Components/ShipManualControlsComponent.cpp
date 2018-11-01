#include "stdafx.h"
#include "ShipManualControlsComponent.h"


ShipManualControlsComponent::ShipManualControlsComponent()
    : AbsComponent(ComponentTypes::ShipManualControls)
{
}


ShipManualControlsComponent::~ShipManualControlsComponent()
{
}

void ShipManualControlsComponent::update(double elapsed)
{
    auto thrustController = owner->getComponent<ThrustControllerComponent>(ComponentTypes::ThrustController);
    if (!owner->hasComponent(ComponentTypes::Focus)) {
        thrustController->setLinearThrust(
            glm::dvec3(0.0)
        );
        thrustController->setAngularThrust(
            glm::dvec3(0.0)
        );
        return;
    }
    auto controls = GameContainer::getInstance()->getControls();
    auto linear_thrust_controls = glm::dvec3(
        controls->readAxisValue("united_controller_manual_linear_left_right_axis"),
        controls->readAxisValue("united_controller_manual_linear_up_down_axis"),
        controls->readAxisValue("united_controller_manual_linear_forward_backward_axis")
    );
    auto angular_thrust_controls = glm::dvec3(
        controls->readAxisValue("united_controller_manual_angular_pitch_axis"),
        controls->readAxisValue("united_controller_manual_angular_yaw_axis"),
        controls->readAxisValue("united_controller_manual_angular_roll_axis")
    );
    if (glm::length(linear_thrust_controls) > 0.001) {
        thrustController->setLinearThrust(
            linear_thrust_controls
        );
    }
    if (glm::length(angular_thrust_controls) > 0.001) {
        thrustController->setAngularThrust(
            angular_thrust_controls
        );
    }
}

AbsComponent * ShipManualControlsComponent::clone()
{
    return new ShipManualControlsComponent();
}

std::string ShipManualControlsComponent::serialize()
{
    std::stringstream s;
    s << serializeBase();
    return s.str();
}

ShipManualControlsComponent * ShipManualControlsComponent::deserialize(std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    auto component = new ShipManualControlsComponent();
    component->deserializeBaseInPlace(serializedString);
    return component;
}
