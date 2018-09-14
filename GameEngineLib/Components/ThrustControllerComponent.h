#pragma once
#include "AbsComponent.h"
class ThrustControllerComponent : public AbsComponent
{
public:
    ThrustControllerComponent();
    ~ThrustControllerComponent();

    void setLinearThrust(glm::dvec3 lt);
    void setAngularThrust(glm::dvec3 at);

    virtual void update(double elapsed) override;
    virtual ThrustControllerComponent * clone() override;

    virtual std::string serialize() override;
    static ThrustControllerComponent* deserialize(std::string serializedString);

private:

    glm::dvec3 linearThrust{ glm::dvec3(0.0) };
    glm::dvec3 angularThrust{ glm::dvec3(0.0) };
};

