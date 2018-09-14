#pragma once
#include "AbsComponent.h"
class Transformation3DComponent : public AbsComponent
{
public: 
    virtual void update(double elapsed) override;
    virtual Transformation3DComponent * clone() override;
    
    Transformation3DComponent(double mass, glm::dvec3 position, glm::dquat orientation, glm::dvec3 linearVelocity, glm::dvec3 angularVelocity);
    Transformation3DComponent(double mass, glm::dvec3 position, glm::dquat orientation, glm::dvec3 linearVelocity);
    Transformation3DComponent(double mass, glm::dvec3 position, glm::dquat orientation);
    Transformation3DComponent(double mass, glm::dvec3 position);
    ~Transformation3DComponent();

    double getMass();
    glm::dvec3 getPosition();
    glm::dquat getOrientation();
    glm::dvec3 predictPosition(double time_delta);
    glm::dquat predictOrientation(double time_delta);
    void setMass(double v);
    void setPosition(glm::dvec3 v);
    void setOrientation(glm::dquat r);
    glm::dvec3 getLinearVelocity();
    glm::dvec3 getAngularVelocity();
    void setLinearVelocity(glm::dvec3 v);
    void setAngularVelocity(glm::dvec3 v);
    void applyImpulse(glm::dvec3 relativePos, glm::dvec3 force);
    void applyAbsoluteImpulse(glm::dvec3 relativePos, glm::dvec3 force);
    void applyGravity(glm::dvec3 force);
    glm::dvec3 modelSpaceToWorld(glm::dvec3 v);
    void setTimeScale(double scale);

    virtual std::string serialize() override;
    static Transformation3DComponent* deserialize(std::string serializedString);

    // bool hitRayPosition(glm::dvec3 origin, glm::dvec3 direction, glm::dvec3 &outpos, glm::dvec3 &outnormal);
    // glm::dvec3 closestSurface(glm::dvec3 position);
private:
    glm::dvec3 position = glm::dvec3(0.0);
    glm::dquat orientation = glm::dquat(1.0, 0.0, 0.0, 0.0);
    glm::dvec3 linearVelocity = glm::dvec3(0.0);
    glm::dvec3 gravityAcceleration = glm::dvec3(0.0);
    glm::dvec3 angularVelocity = glm::dvec3(0.0);
    double timeScale = 0.001;
    double mass;
    //Object3dInfo * collision3dInfo;
};

