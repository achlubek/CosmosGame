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

