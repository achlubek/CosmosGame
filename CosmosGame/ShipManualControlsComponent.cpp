#include "stdafx.h"
#include "ShipManualControlsComponent.h"
#include "GameContainer.h"
#include "GameControls.h"
#include "ThrustControllerComponent.h"


ShipManualControlsComponent::ShipManualControlsComponent()
    : AbsComponent(ComponentTypes::ShipManualControls)
{
}


ShipManualControlsComponent::~ShipManualControlsComponent()
{
}

void ShipManualControlsComponent::update(double elapsed)
{
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
    auto thrustController = owner->getComponent<ThrustControllerComponent>(ComponentTypes::ThrustController);
    thrustController->setLinearThrust(
        linear_thrust_controls
    );
    thrustController->setAngularThrust(
        angular_thrust_controls
    );
}

void ShipManualControlsComponent::loadFromFile(std::string mediakey)
{
}

AbsComponent * ShipManualControlsComponent::clone()
{
    return new ShipManualControlsComponent();
}
