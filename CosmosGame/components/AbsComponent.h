#pragma once
#include "ComponentTypes.h"
#include "../GameObject.h"
class AbsComponent
{
public:
    AbsComponent(ComponentTypes type);
    ~AbsComponent();
    ComponentTypes getType();
    unsigned long getID();
    virtual void update(double elapsed) = 0;
    virtual void loadFromFile(std::string mediakey) = 0;
    void setOwner(GameObject* obj);
protected:
    GameObject* owner;
private:
    ComponentTypes type;
    static unsigned long idSharedCounter;
    unsigned long id;
};

