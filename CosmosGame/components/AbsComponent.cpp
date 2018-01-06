#include "stdafx.h"
#include "AbsComponent.h"

unsigned long AbsComponent::idSharedCounter = 0;

AbsComponent::AbsComponent(ComponentTypes itype)
    : id(idSharedCounter++), type(itype)
{

}


AbsComponent::~AbsComponent()
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

void AbsComponent::setOwner(GameObject * obj)
{
    owner = obj;
}
