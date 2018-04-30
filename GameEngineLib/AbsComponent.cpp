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

void AbsComponent::setOwner(GameObject * obj)
{
    owner = obj;
}

bool AbsComponent::isDrawable()
{
    return false;
}
