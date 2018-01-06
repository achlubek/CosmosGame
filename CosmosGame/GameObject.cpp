#include "stdafx.h"
#include "GameObject.h"
#include "components\AbsComponent.h"

unsigned long GameObject::idSharedCounter = 0;

GameObject::GameObject()
    : id(idSharedCounter++), components({})
{
}


GameObject::~GameObject()
{
    removeAllComponents();
}

void GameObject::addComponent(AbsComponent * component)
{
    components.push_back(component);
}

void GameObject::removeComponent(AbsComponent * component)
{
    auto found = std::find(components.begin(), components.end(), component);
    if (found != components.end()) {
        components.erase(found);
    }
    delete component;
}

void GameObject::removeAllComponents()
{
    for (int i = 0; i < components.size(); i++) {
        delete components[i];
    }
    components.clear();
}

void GameObject::removeComponentsByType(ComponentTypes type)
{
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i]->getType() == type)
        {
            delete components[i];
            components[i] = components[components.size() - 1];
            components.pop_back();
            --i;
        }
    }
}

unsigned long GameObject::getID()
{
    return id;
}
