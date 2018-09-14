#include "stdafx.h"
#include "ThrustControllerComponent.h"


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

        case ThrustGroup::forward:
            gen->setPowerPercentage(linearThrust.z);
            break;
        case ThrustGroup::backward:
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

ThrustControllerComponent * ThrustControllerComponent::clone()
{
    return nullptr;
}

std::string ThrustControllerComponent::serialize()
{
    std::stringstream s;
    s << serializeBase();
    s << "linearThrust=" << linearThrust.x << " " << linearThrust.y << " " << linearThrust.z << "\n";
    s << "angularThrust=" << angularThrust.x << " " << angularThrust.y << " " << angularThrust.z << "\n";
    return s.str();
}

ThrustControllerComponent * ThrustControllerComponent::deserialize(std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    auto component = new ThrustControllerComponent();
    component->setLinearThrust(reader.getdv3("linearThrust"));
    component->setAngularThrust(reader.getdv3("angularThrust"));
    component->deserializeBaseInPlace(serializedString);
    return component;
}

void ThrustControllerComponent::setLinearThrust(glm::dvec3 lt)
{
    linearThrust = lt;
}

void ThrustControllerComponent::setAngularThrust(glm::dvec3 at)
{
    angularThrust = at;
}