#include "stdafx.h"
#include "ThrustGeneratorComponent.h"


ThrustGeneratorComponent::ThrustGeneratorComponent(Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation)
    : AbsDrawableComponent(ComponentTypes::ThrustGenerator, model, relativePosition, relativeOrientation)
{
}


ThrustGeneratorComponent::~ThrustGeneratorComponent()
{
}

void ThrustGeneratorComponent::update(double elapsed)
{
}

void ThrustGeneratorComponent::loadFromFile(std::string mediakey)
{
}

ThrustGeneratorComponent * ThrustGeneratorComponent::clone()
{
    return new ThrustGeneratorComponent(model, relativePosition, relativeOrientation);
}