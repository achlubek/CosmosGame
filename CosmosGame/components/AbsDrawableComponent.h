#pragma once
#include "AbsComponent.h"
class Model3d;
class AbsDrawableComponent : public AbsComponent
{
public:
    AbsDrawableComponent(ComponentTypes type, Model3d* model, glm::dvec3 relativePosition, glm::dquat relativeOrientation);
    ~AbsDrawableComponent();
    virtual bool isDrawable() override;
    void draw(glm::dvec3 observerPosition);
private:
    glm::dvec3 relativePosition;
    glm::dquat relativeOrientation;
    Model3d* model;
};

