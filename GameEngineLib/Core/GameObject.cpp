#include "stdafx.h"
#include "GameObject.h"

unsigned long GameObject::idSharedCounter = 0;

GameObject::GameObject()
    : id(idSharedCounter++), components({}), tags({})
{
}


GameObject::~GameObject()
{
    removeAllComponents();
}

void GameObject::addTag(GameObjectTags tag)
{
    tags.push_back(tag);
}

void GameObject::removeTag(GameObjectTags tag)
{
    auto found = std::find(tags.begin(), tags.end(), tag);
    if (found != tags.end()) {
        tags.erase(found);
    }
}

bool GameObject::hasTag(GameObjectTags tag)
{
    for (int i = 0; i < tags.size(); ++i)
    {
        if (tags[i] == tag)
        {
            return true;
        }
    }
    return false;
}

std::vector<GameObjectTags> GameObject::getAllTags()
{
    return tags;
}

void GameObject::addComponent(AbsComponent * component)
{
    component->setOwner(this);
    components.push_back(component);
}

void GameObject::removeComponent(AbsComponent * component)
{
    auto found = std::find(components.begin(), components.end(), component);
    if (found != components.end()) {
        components.erase(found);
    }
    delete component; // TODO smart pointers...
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
    for (int i = 0; i < components.size(); i++) {
        if (components[i]->getType() == type) {
            delete components[i];
            components[i] = components[components.size() - 1];
            components.pop_back();
            --i;
        }
    }
}

void GameObject::update(double elapsed)
{
    for (int i = 0; i < components.size(); i++) {
        components[i]->update(elapsed);
    }
}

GameObject * GameObject::clone()
{
    auto copy = new GameObject();
    for (int i = 0; i < components.size(); i++) {
        copy->addComponent(components[i]->clone());
    }
    return copy;
}

unsigned long GameObject::getID()
{
    return id;
}

void GameObject::setID(unsigned long idValue)
{
    id = idValue;
}

bool GameObject::hasComponent(ComponentTypes type)
{
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i]->getType() == type)
        {
            return true;
        }
    }
    return false;
}

std::vector<AbsComponent*> GameObject::getAllComponents()
{
    return components;
}


void GameObject::restoreSharedCounterValue(unsigned long idSharedCounterValue)
{
    idSharedCounter = idSharedCounterValue;
}

unsigned long GameObject::getSharedCounterValue()
{
    return idSharedCounter;
}
