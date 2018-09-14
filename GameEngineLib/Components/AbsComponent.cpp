#include "stdafx.h"
#include "AbsComponent.h"

unsigned long AbsComponent::idSharedCounter = 1;

AbsComponent::AbsComponent(ComponentTypes itype)
    : id(idSharedCounter++), type(itype)
{

}


ComponentTypes AbsComponent::getType()
{
    return type;
}

unsigned long AbsComponent::getID()
{
    return id;
}

void AbsComponent::setID(unsigned long idValue)
{
    id = idValue;
}

void AbsComponent::setOwner(GameObject * obj)
{
    owner = obj;
}

bool AbsComponent::isDrawable()
{
    return false;
}

void AbsComponent::restoreSharedCounterValue(unsigned long idSharedCounterValue)
{
    idSharedCounter = idSharedCounterValue;
}

unsigned long AbsComponent::getSharedCounterValue()
{
    return idSharedCounter;
}

std::string AbsComponent::serializeBase()
{
    std::stringstream s;
    s << "component_type=" << static_cast<int32_t>(type) << "\n";
    s << "id=" << id << "\n";
    return s.str();
}

void AbsComponent::deserializeBaseInPlace(std::string string)
{
    INIReader reader = INIReader(string);
    id = reader.geti64("id");
    type = static_cast<ComponentTypes>(reader.geti("component_type"));
}
