#pragma once
#include "../Components/AbsComponent.h"
#include "../Components/ComponentTypes.h"
#include "GameObjectTags.h"
class GameObject
{
public:
    GameObject();
    ~GameObject();

    void addTag(GameObjectTags tag);
    void removeTag(GameObjectTags tag);
    bool hasTag(GameObjectTags tag);
    std::vector<GameObjectTags> getAllTags();

    void addComponent(AbsComponent* component);
    void removeComponent(AbsComponent* component);
    void removeAllComponents();
    void removeComponentsByType(ComponentTypes type);
    void update(double elapsed);
    GameObject* clone();
    unsigned long getID();
    void setID(unsigned long id);
    bool hasComponent(ComponentTypes type);
    std::vector<AbsComponent*> getAllComponents();
    template<class T>
    T* getComponent(ComponentTypes type);
    template<class T>
    std::vector<T*> getAllComponentsByType(ComponentTypes type);

    static void restoreSharedCounterValue(unsigned long idSharedCounterValue);
    static unsigned long getSharedCounterValue();

private:
    std::vector<AbsComponent*> components;
    std::vector<GameObjectTags> tags;
    static unsigned long idSharedCounter;
    unsigned long id;
};

template<typename T>
inline T * GameObject::getComponent(ComponentTypes type)
{
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i]->getType() == type)
        {
            return static_cast<T*>(components[i]);
        }
    }
    return nullptr;
}

template<class T>
inline std::vector<T*> GameObject::getAllComponentsByType(ComponentTypes type)
{
    std::vector<T*> res = {};
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i]->getType() == type)
        {
            res.push_back(static_cast<T*>(components[i]));
        }
    }
    return res;
}
