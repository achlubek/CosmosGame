#include "stdafx.h"
#include "AbsDrawableComponent.h"
#include "Transformation3DComponent.h"
#include "ComponentTypes.h"
#include "AbsGameContainer.h"
#include "Model3d.h"


AbsDrawableComponent::AbsDrawableComponent(Model3d* imodel, glm::dvec3 irelativePosition, glm::dquat irelativeOrientation)
    : AbsComponent(ComponentTypes::SimpleDrawable), model(imodel), relativePosition(irelativePosition), relativeOrientation(irelativeOrientation)
{
}

AbsDrawableComponent::AbsDrawableComponent(ComponentTypes type, Model3d* imodel, glm::dvec3 irelativePosition, glm::dquat irelativeOrientation)
    : AbsComponent(type), model(imodel), relativePosition(irelativePosition), relativeOrientation(irelativeOrientation)
{
}

bool AbsDrawableComponent::isDrawable()
{
    return true;
}

void AbsDrawableComponent::draw(glm::dvec3 observerPosition, VulkanRenderStage* stage, VulkanDescriptorSet* set, double scale)
{
    Transformation3DComponent* parentTransform = owner->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);

    auto pos = parentTransform->getPosition() - observerPosition;
    auto m3_rot = glm::mat3_cast(parentTransform->getOrientation());

    auto modulepos = pos + m3_rot * relativePosition * 0.001;
    auto moduleort = parentTransform->getOrientation() * relativeOrientation * glm::angleAxis((double)3.1415 * 0.5, glm::dvec3(1.0, 0.0, 0.0));
    model->draw(stage, set, modulepos, moduleort, scale);
}

void AbsDrawableComponent::update(double elapsed)
{
}

void AbsDrawableComponent::loadFromFile(std::string mediakey)
{
}

AbsDrawableComponent * AbsDrawableComponent::clone()
{ 
    return new AbsDrawableComponent(getType(), model, relativePosition, relativeOrientation);
}

void AbsDrawableComponent::setRelativePosition(glm::dvec3 irelativePosition)
{
    relativePosition = irelativePosition;
}

glm::dvec3 AbsDrawableComponent::getRelativePosition()
{
    return relativePosition;
}

void AbsDrawableComponent::setRelativeOrientation(glm::dquat irelativeOrientation)
{
    relativeOrientation = irelativeOrientation;
}

glm::dquat AbsDrawableComponent::getRelativeOrientation()
{
    return relativeOrientation;
}
