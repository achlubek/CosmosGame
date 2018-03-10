#pragma once
class AbsComponent;
#include "ComponentTypes.h"
class GameObject
{
public:
    GameObject();
    ~GameObject();
    void addComponent(AbsComponent* component);
    void removeComponent(AbsComponent* component);
    void removeAllComponents();
    void removeComponentsByType(ComponentTypes type);
    void update(double elapsed);
    GameObject* clone();
    unsigned long getID();
    std::vector<AbsComponent*> getAllComponents();
    template<class T>
    T* getComponent(ComponentTypes type);
    template<class T>
    std::vector<T*> getAllComponentsByType(ComponentTypes type);
    bool isDead();
    void die();
private:
    std::vector<AbsComponent*> components;
    static unsigned long idSharedCounter;
    unsigned long id;
    bool dead{ false };
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
