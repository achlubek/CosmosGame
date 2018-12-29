#include "stdafx.h"
#include "ShipFactory.h"
#include "../json.h"

using json = nlohmann::json;


ShipFactory::ShipFactory(Model3dFactory* imodel3dFactory, ModuleFactory* imoduleFactory, MediaInterface* media)
    : model3dFactory(imodel3dFactory), moduleFactory(imoduleFactory), media(media)
{
}


ShipFactory::~ShipFactory()
{
}

glm::dquat vec3toquat(glm::dvec3 dir, double angle = 0) {

    glm::dvec3 up = glm::dvec3(0.0, 1.0, 0.0);
    if (glm::dot(up, dir) > 0.9999999) {
        up = glm::dvec3(1.0, 0.0, 0.0);
    }
    if (glm::dot(up, -dir) > 0.9999999) {
        up = glm::dvec3(1.0, 0.0, 0.0);
    }
    // lets go full linear algebra here
    auto cr1 = glm::normalize(glm::cross(dir, up));
    auto cr2 = glm::normalize(glm::cross(dir, cr1));

    glm::dmat3 m = glm::dmat3(cr1, cr2, dir);
    return glm::angleAxis(angle, dir) * glm::quat_cast(m);
}

GameObject * ShipFactory::build(std::string mediakey)
{
    //INIReader reader = INIReader(mediakey);

    auto data = json::parse(media->readString(mediakey));

    GameObject* ship = new GameObject();

    for (auto& element : data["models"]) {
        auto model3d = model3dFactory->build(element);
        auto drawableComponent = new AbsDrawableComponent(model3d, element, glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0));
        drawableComponent->setEmissionPercentage(1.0);
        ship->addComponent(drawableComponent);
    }

    auto transformComponent = new Transformation3DComponent(data["mass"], glm::dvec3(0.0));
    ship->addComponent(transformComponent);

    std::unordered_map<string, int> functionalityMap = {};
    functionalityMap["forward"] = 0;
    functionalityMap["backward"] = 1;
    functionalityMap["left"] = 2;
    functionalityMap["right"] = 3;
    functionalityMap["up"] = 4;
    functionalityMap["down"] = 5;
    functionalityMap["pitch_up"] = 6;
    functionalityMap["pitch_down"] = 7;
    functionalityMap["roll_left"] = 8;
    functionalityMap["roll_right"] = 9;
    functionalityMap["yaw_left"] = 10;
    functionalityMap["yaw_right"] = 11;

    ship->addComponent(new BatteryComponent(100000, 100000));

    for (auto& element : data["modules"]) {
        auto component = moduleFactory->build(element["module"]);
        if (component->isDrawable()) {
            auto drawable = static_cast<AbsDrawableComponent*>(component);
            drawable->setRelativePosition(glm::dvec3(element["pos"]["x"], element["pos"]["y"], element["pos"]["z"]));
            drawable->setRelativeOrientation(vec3toquat(glm::dvec3(element["rot"]["x"], element["rot"]["y"], element["rot"]["z"]), element["dirrot"]));
        }
        if (component->getType() == ComponentTypes::ThrustGenerator) {
            static_cast<ThrustGeneratorComponent*>(component)->functionalityGroup = static_cast<ThrustGroup>(functionalityMap[element["link"]]);
        }
        ship->addComponent(component);
    }
    ship->addComponent(new ThrustControllerComponent());
    ship->addComponent(new ShipManualControlsComponent());
    ship->addComponent(new ShipAutopilotComponent());

    return ship;
}
