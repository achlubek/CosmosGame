#include "stdafx.h"
#include "ShipFactory.h"
#include "Model3d.h"
#include "GameContainer.h"
#include "GameObject.h"
#include "SQLiteDatabase.h"
#include "CosmosRenderer.h"
#include "Transformation3DComponent.h"
#include "AbsDrawableComponent.h"
#include "ThrustGeneratorComponent.h"
#include "ThrustControllerComponent.h"
#include "ShipManualControlsComponent.h"
#include "BatteryComponent.h"
#include "Model3dFactory.h"
#include "ModuleFactory.h"


ShipFactory::ShipFactory(Model3dFactory* imodel3dFactory, ModuleFactory* imoduleFactory)
    : model3dFactory(imodel3dFactory), moduleFactory(imoduleFactory)
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
    INIReader reader = INIReader(mediakey);
    GameObject* ship = new GameObject();

    auto model3d = model3dFactory->build(reader.gets("model3d"));

    auto drawableComponent = new AbsDrawableComponent(model3d, glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0));
    auto transformComponent = new Transformation3DComponent(reader.getf("mass"), glm::dvec3(0.0));
    ship->addComponent(drawableComponent);
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

    int modulesCount = reader.geti("modules_count");

    for (int i = 0; i < modulesCount; i++) {
        std::string prefix = "module_" + std::to_string(i) + ".";
        auto component = moduleFactory->build(reader.gets(prefix + "module"));
        if (component->isDrawable()) {
            auto drawable = static_cast<AbsDrawableComponent*>(component);
            drawable->setRelativePosition(reader.getv3(prefix + "pos"));
            drawable->setRelativeOrientation(vec3toquat(reader.getv3(prefix + "rot"), reader.getf(prefix + "dirrot")));
        }
        if (component->getType() == ComponentTypes::ThrustGenerator) {
            static_cast<ThrustGeneratorComponent*>(component)->functionalityGroup = static_cast<ThrustGroup>(functionalityMap[reader.gets(prefix + "link_name")]);
        }
        ship->addComponent(component);
    }
    ship->addComponent(new ThrustControllerComponent());
    ship->addComponent(new ShipManualControlsComponent());

    return ship;
}
