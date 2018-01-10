#pragma once
class AbsComponent;
#include "components\ComponentTypes.h"
class GameObject
{
public:
    GameObject();
    ~GameObject();
    void addComponent(AbsComponent* component);
    void removeComponent(AbsComponent* component);
    void removeAllComponents();
    void removeComponentsByType(ComponentTypes type);
    GameObject* clone();
    unsigned long getID();
    template<class T>
    T* getComponent(ComponentTypes type);
    template<class T>
    std::vector<T*> getAllComponentsByType(ComponentTypes type);
private:
    std::vector<AbsComponent*> components;
    static unsigned long idSharedCounter;
    unsigned long id;
};

template<class T>
inline T * GameObject::getComponent(ComponentTypes type)
{
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i]->getType() == type)
        {
            return components[i];
        }
    }
}

template<class T>
inline std::vector<T*> GameObject::getAllComponentsByType(ComponentTypes type)
{
    std::vector<T*> res = {};
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i]->getType() == type)
        {
            res.push_back(components[i]);
        }
    }
    return res;
}
