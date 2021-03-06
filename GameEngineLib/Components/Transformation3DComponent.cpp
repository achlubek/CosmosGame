#include "stdafx.h"
#include "Transformation3DComponent.h"
 
void Transformation3DComponent::update(double elapsed)
{
    auto time = GameContainer::getInstance()->getCurrentStage()->getTimeProvider()->getTime();
    while (elapsed > 0.01) {
        auto gravityAcceleration = GameContainer::getInstance()->getCosmosRenderer()->getGalaxy()->getGravity(position, time);
        linearVelocity += gravityAcceleration * 0.01;
        position = predictPosition(0.01 * timeScale);
        orientation = predictOrientation(0.01);
        elapsed -= 0.01;
    }
    auto gravityAcceleration = GameContainer::getInstance()->getCosmosRenderer()->getGalaxy()->getGravity(position, time);
    linearVelocity += gravityAcceleration * elapsed;
    position = predictPosition(elapsed * timeScale);
    orientation = predictOrientation(elapsed);
}

Transformation3DComponent * Transformation3DComponent::clone()
{
    auto res = new Transformation3DComponent(mass, position, orientation, linearVelocity, angularVelocity);
    res->timeScale = timeScale;
    return res;
}

Transformation3DComponent::Transformation3DComponent(double imass, glm::dvec3 iposition, glm::dquat iorientation, glm::dvec3 ilinearVelocity, glm::dvec3 iangularVelocity)
    : AbsComponent(ComponentTypes::Transformation3D), mass(imass), position(iposition), orientation(iorientation), linearVelocity(ilinearVelocity), angularVelocity(iangularVelocity)
{
}

Transformation3DComponent::Transformation3DComponent(double imass, glm::dvec3 iposition, glm::dquat iorientation, glm::dvec3 ilinearVelocity)
    : AbsComponent(ComponentTypes::Transformation3D), mass(imass), position(iposition), orientation(iorientation), linearVelocity(ilinearVelocity)
{
}

Transformation3DComponent::Transformation3DComponent(double imass, glm::dvec3 iposition, glm::dquat iorientation)
    : AbsComponent(ComponentTypes::Transformation3D), mass(imass), position(iposition), orientation(iorientation)
{
}

Transformation3DComponent::Transformation3DComponent(double imass, glm::dvec3 iposition)
    : AbsComponent(ComponentTypes::Transformation3D), mass(imass), position(iposition)
{
}

Transformation3DComponent::~Transformation3DComponent()
{
}

double Transformation3DComponent::getMass()
{
    return mass;
}

glm::dvec3 Transformation3DComponent::getPosition()
{
    return position;
}

glm::dquat Transformation3DComponent::getOrientation()
{
    return orientation;
}

glm::dvec3 Transformation3DComponent::predictPosition(double time_delta)
{
    return position + time_delta * linearVelocity;
}

glm::dquat Transformation3DComponent::predictOrientation(double time_delta)
{
    return orientation
        * glm::angleAxis(time_delta * angularVelocity.x, glm::dvec3(1.0, 0.0, 0.0))
        * glm::angleAxis(time_delta * angularVelocity.y, glm::dvec3(0.0, 1.0, 0.0))
        * glm::angleAxis(time_delta * angularVelocity.z, glm::dvec3(0.0, 0.0, 1.0));
}

void Transformation3DComponent::setMass(double v)
{
    mass = v;
}

void Transformation3DComponent::setPosition(glm::dvec3 v)
{
    position = v;
}

void Transformation3DComponent::setOrientation(glm::dquat r)
{
    orientation = r;
}

glm::dvec3 Transformation3DComponent::getLinearVelocity()
{
    return linearVelocity;
}

glm::dvec3 Transformation3DComponent::getAngularVelocity()
{
    return angularVelocity;
}

void Transformation3DComponent::setLinearVelocity(glm::dvec3 v)
{
    linearVelocity = v;
}

void Transformation3DComponent::setAngularVelocity(glm::dvec3 v)
{
    angularVelocity = v;
}
 
void Transformation3DComponent::applyImpulse(glm::dvec3 relativePos, glm::dvec3 force)
{
    if (glm::length(relativePos) > 0.0) {
        double cosine = glm::length(force) == 0 ? 0.0 : glm::dot(glm::normalize(relativePos), glm::normalize(force));
        double sine = glm::sign(cosine) * glm::sqrt(1.0 - cosine * cosine);
        double t = glm::length(relativePos) * glm::length(force) * sine;
        glm::dvec3 T = glm::cross(relativePos, force);
        angularVelocity += T / mass;
    }
    auto m3 = glm::mat3_cast(orientation);
    linearVelocity += m3 * (/*glm::abs(cosine) * */ force / mass); // 100% when relativepos == force (dot == 1) or =relativepos == force (dot == -1), 0 when dot == 0
                                                                   // it just didnt fit....
}

void Transformation3DComponent::applyAbsoluteImpulse(glm::dvec3 relativePos, glm::dvec3 force)
{
    if (glm::length(relativePos) > 0.0) {
        double cosine = glm::length(force) == 0 ? 0.0 : glm::dot(glm::normalize(relativePos), glm::normalize(force));
        double sine = glm::sign(cosine) * glm::sqrt(1.0 - cosine * cosine);
        double t = glm::length(relativePos) * glm::length(force) * sine;
        glm::dvec3 T = glm::cross(relativePos, force);
        angularVelocity += T / mass;
    }
    linearVelocity += (/*glm::abs(cosine) * */ force / mass); // 100% when relativepos == force (dot == 1) or =relativepos == force (dot == -1), 0 when dot == 0
                                                              // it just didnt fit....
}

glm::dvec3 Transformation3DComponent::modelSpaceToWorld(glm::dvec3 v)
{
    glm::dmat3 shipmat = glm::mat3_cast(orientation);
    return shipmat * v + position;
}

void Transformation3DComponent::setTimeScale(double scale)
{
    timeScale = scale;
}

std::string Transformation3DComponent::serialize()
{
    std::stringstream s;
    s << serializeBase();
    s << std::setprecision(8);
    s << "position=" << position.x << " " << position.y << " " << position.z << "\n";
    s << "orientation=" << orientation.w << " " << orientation.x << " " << orientation.y << " " << orientation.z << "\n";
    s << "linearVelocity=" << linearVelocity.x << " " << linearVelocity.y << " " << linearVelocity.z << "\n";
    s << "angularVelocity=" << angularVelocity.x << " " << angularVelocity.y << " " << angularVelocity.z << "\n";
    s << "timeScale=" << timeScale << "\n";
    s << "mass=" << mass << "\n";
    return s.str();
}

Transformation3DComponent * Transformation3DComponent::deserialize(std::string serializedString)
{
    INIReader reader = INIReader(serializedString);
    auto component = new Transformation3DComponent(
        reader.getd("mass"),
        reader.getdv3("position"),
        reader.getdquat("orientation"),
        reader.getdv3("linearVelocity"),
        reader.getdv3("angularVelocity")
    );
    component->setTimeScale(reader.getd("timeScale"));
    component->deserializeBaseInPlace(serializedString);
    return component;
}
