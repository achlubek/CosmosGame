#include "stdafx.h"
#include "FocusComponent.h"


FocusComponent::FocusComponent()
    : AbsComponent(ComponentTypes::Focus)
{
}


FocusComponent::~FocusComponent()
{
}

void FocusComponent::update(double elapsed)
{
}

AbsComponent * FocusComponent::clone()
{
    return new FocusComponent();
}

std::string FocusComponent::serialize()
{
    std::stringstream s;
    s << serializeBase();
    return s.str();
}

FocusComponent * FocusComponent::deserialize(std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    auto component = new FocusComponent();
    component->deserializeBaseInPlace(serializedString);
    return component;
}
