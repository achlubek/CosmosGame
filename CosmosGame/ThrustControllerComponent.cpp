#include "stdafx.h"
#include "ThrustControllerComponent.h"
#include "ThrustGeneratorComponent.h"


ThrustControllerComponent::ThrustControllerComponent()
    : AbsComponent(ComponentTypes::ThrustController)
{
}


ThrustControllerComponent::~ThrustControllerComponent()
{
}

void ThrustControllerComponent::update(double elapsed)
{
    std::vector<ThrustGeneratorComponent*> thrustGens = owner->getAllComponentsByType<ThrustGeneratorComponent>(ComponentTypes::ThrustGenerator);
    for (int i = 0; i < thrustGens.size(); i++) {
        auto gen = thrustGens[i];
        switch (gen->functionalityGroup) {

        case ThrustGroup::backward:
            gen->setPowerPercentage(linearThrust.z);
            break;
        case ThrustGroup::forward:
            gen->setPowerPercentage(-linearThrust.z);
            break;
        case ThrustGroup::up:
            gen->setPowerPercentage(linearThrust.y);
            break;
        case ThrustGroup::down:
            gen->setPowerPercentage(-linearThrust.y);
            break;
        case ThrustGroup::left:
            gen->setPowerPercentage(linearThrust.x);
            break;
        case ThrustGroup::right:
            gen->setPowerPercentage(-linearThrust.x);
            break;


        case ThrustGroup::pitch_up:
            gen->setPowerPercentage(angularThrust.x);
            break;
        case ThrustGroup::pitch_down:
            gen->setPowerPercentage(-angularThrust.x);
            break;
        case ThrustGroup::roll_left:
            gen->setPowerPercentage(angularThrust.z);
            break;
        case ThrustGroup::roll_right:
            gen->setPowerPercentage(-angularThrust.z);
            break;
        case ThrustGroup::yaw_left:
            gen->setPowerPercentage(angularThrust.y);
            break;
        case ThrustGroup::yaw_right:
            gen->setPowerPercentage(-angularThrust.y);
            break;

        }
    }
}

void ThrustControllerComponent::loadFromFile(std::string mediakey)
{
}

ThrustControllerComponent * ThrustControllerComponent::clone()
{
    return nullptr;
}

void ThrustControllerComponent::setLinearThrust(glm::dvec3 lt)
{
    linearThrust = lt;
}

void ThrustControllerComponent::setAngularThrust(glm::dvec3 at)
{
    angularThrust = at;
}