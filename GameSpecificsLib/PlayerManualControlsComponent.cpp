#include "stdafx.h"
#include "PlayerManualControlsComponent.h"
#include "GameContainer.h"
#include "GameControls.h"
#include "Transformation3DComponent.h"

PlayerManualControlsComponent::PlayerManualControlsComponent()
    : AbsComponent(ComponentTypes::PlayerManualControls)
{
}


PlayerManualControlsComponent::~PlayerManualControlsComponent()
{
}

void PlayerManualControlsComponent::update(double elapsed)
{
    auto controls = GameContainer::getInstance()->getControls();
    auto linear_controls = glm::dvec3(
        controls->readAxisValue("player_inperson_linear_left_right_axis"),
        controls->readAxisValue("player_inperson_linear_up_down_axis"),
        -controls->readAxisValue("player_inperson_linear_forward_backward_axis")
    );    
    auto angular_controls = glm::dvec3(
        -controls->readAxisValue("player_inperson_angular_pitch_axis"),
        -controls->readAxisValue("player_inperson_angular_yaw_axis"),
        -controls->readAxisValue("player_inperson_angular_roll_axis")
    );
    auto physicsComponent = owner->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);
    auto rotation = physicsComponent->getOrientation();
    auto linearVector = glm::mat3_cast(rotation) * linear_controls;

    physicsComponent->setAngularVelocity(angular_controls * 0.1 + physicsComponent->getAngularVelocity() * 0.9); // todo make it based on time
    physicsComponent->setLinearVelocity(linearVector + physicsComponent->getLinearVelocity());
}

AbsComponent * PlayerManualControlsComponent::clone()
{
    return new PlayerManualControlsComponent();
}
