#pragma once
#include "AbsComponent.h"
#include "ComponentTypes.h"
class ThrustControllerComponent : public AbsComponent
{
public:
    ThrustControllerComponent();
    ~ThrustControllerComponent();

    void setLinearThrust(glm::dvec3 lt);
    void setAngularThrust(glm::dvec3 at);

    virtual void update(double elapsed) override;
    virtual void loadFromFile(std::string mediakey) override;
    virtual ThrustControllerComponent * clone() override;
private:

    glm::dvec3 linearThrust;
    glm::dvec3 angularThrust;
};

