#pragma once
#include "ComponentTypes.h"
class GameObject;
class AbsComponent
{
public:
    AbsComponent(ComponentTypes type);
    virtual ~AbsComponent() {};
    ComponentTypes getType();
    unsigned long getID();
    void setID(unsigned long id);
    virtual void update(double elapsed) = 0;
    virtual AbsComponent* clone() = 0;
    void setOwner(GameObject* obj);
    virtual bool isDrawable();
    static void restoreSharedCounterValue(unsigned long idSharedCounterValue);
    static unsigned long getSharedCounterValue();

    virtual std::string serialize() = 0;

protected:
    GameObject* owner;
    unsigned long id;

    std::string serializeBase();
    void deserializeBaseInPlace(std::string string);

private:
    ComponentTypes type;
    static unsigned long idSharedCounter;
};

