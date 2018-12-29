#include "stdafx.h"
#include "AbsDrawableComponent.h"


AbsDrawableComponent::AbsDrawableComponent(Model3d* imodel, std::string modelName, glm::dvec3 irelativePosition, glm::dquat irelativeOrientation)
    : AbsComponent(ComponentTypes::SimpleDrawable), model(imodel), modelName(modelName), relativePosition(irelativePosition), relativeOrientation(irelativeOrientation)
{
}

AbsDrawableComponent::AbsDrawableComponent(ComponentTypes type, Model3d* imodel, std::string modelName, glm::dvec3 irelativePosition, glm::dquat irelativeOrientation)
    : AbsComponent(type), model(imodel), modelName(modelName), relativePosition(irelativePosition), relativeOrientation(irelativeOrientation)
{
}

bool AbsDrawableComponent::isDrawable()
{
    return true;
}

void AbsDrawableComponent::draw(glm::dvec3 observerPosition, RenderStageInterface* stage, DescriptorSetInterface* set, double scale)
{
    Transformation3DComponent* parentTransform = owner->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);

    auto pos = parentTransform->getPosition() - observerPosition;
    auto m3_rot = glm::mat3_cast(parentTransform->getOrientation());

    auto modulepos = pos + m3_rot * relativePosition * 1.0;
    auto moduleort = parentTransform->getOrientation() * relativeOrientation * glm::angleAxis((double)3.1415 * 0.5, glm::dvec3(1.0, 0.0, 0.0));
    model->draw(stage, set, modulepos, moduleort, scale, getID(), emissionValue);
}

glm::dvec3 AbsDrawableComponent::getWorldTranslation()
{
    Transformation3DComponent* parentTransform = owner->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);

    auto m3_rot = glm::mat3_cast(parentTransform->getOrientation());

    return m3_rot * relativePosition * 1.0;
}

void AbsDrawableComponent::update(double elapsed)
{
}

AbsDrawableComponent * AbsDrawableComponent::clone()
{ 
    return new AbsDrawableComponent(getType(), model, modelName, relativePosition, relativeOrientation);
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

void AbsDrawableComponent::setEmissionPercentage(float percentage)
{
    emissionValue = percentage;
}

std::string AbsDrawableComponent::serialize()
{
    std::stringstream s;
    s << serializeBase();
    s << "modelName=" << modelName << "\n";
    s << "emissionValue=" << emissionValue << "\n";
    s << "relativePosition=" << relativePosition.x << " " << relativePosition.y << " " << relativePosition.z << "\n";
    s << "relativeOrientation=" << relativeOrientation.w << " " << relativeOrientation.x << " " << relativeOrientation.y << " " << relativeOrientation.z << "\n";
    return s.str();
}

AbsDrawableComponent * AbsDrawableComponent::deserialize(Model3dFactory * model3dFactory, std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    auto modelName = reader.gets("modelName");
    auto model = model3dFactory->build(modelName);
    auto component = new AbsDrawableComponent(model, modelName, reader.getdv3("relativePosition"), reader.getdquat("relativeOrientation"));
    component->deserializeBaseInPlace(serializedString);
    component->setEmissionPercentage(reader.getd("emissionValue"));
    return component;
}
