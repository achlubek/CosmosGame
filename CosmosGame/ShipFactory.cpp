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


ShipFactory::ShipFactory()
{
}


ShipFactory::~ShipFactory()
{
}

#define asstring(a) std::to_string(a)
#define asdouble(a) std::stod(a)
#define asint(a) std::stoi(a)

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
glm::dquat axes_vec3toquat(glm::dvec3 rot ) {
    glm::mat3 xrot = glm::mat3_cast(glm::angleAxis(deg2rad(rot.x), glm::dvec3(1.0, 0.0, 0.0)));
    glm::mat3 zrot = glm::mat3_cast(glm::angleAxis(deg2rad(rot.y), glm::dvec3(0.0, 1.0, 0.0)));
    glm::mat3 yrot = glm::mat3_cast(glm::angleAxis(deg2rad(rot.z), glm::dvec3(0.0, 0.0, 1.0)));
    return glm::dquat(xrot * yrot * zrot);
}

Model3d * readModel3d(int id)
{
    auto db = GameContainer::getInstance()->getDatabase();
    auto model3d_data = db->query("SELECT * FROM models3d WHERE id = " + asstring(id))[0];
    // TODO this smells a lot
    auto vulkan = GameContainer::getInstance()->getVulkanToolkit();
    auto modeldset = GameContainer::getInstance()->getCosmosRenderer()->modelMRTLayout;
    glm::dquat correction = axes_vec3toquat(glm::dvec3(asdouble(model3d_data["rotx"]), asdouble(model3d_data["roty"]), asdouble(model3d_data["rotz"])));
    auto model3d = new Model3d(vulkan, modeldset, model3d_data["info3d_file"], model3d_data["albedo_image"], model3d_data["normal_image"],
        model3d_data["roughness_image"], model3d_data["metalness_image"], model3d_data["emission_idle_image"], model3d_data["emission_powered_image"], correction);
    return model3d;
}


#define MODULE_TYPE_HYPERDRIVE 1
#define MODULE_TYPE_ENGINE 2

GameObject * ShipFactory::build(int id)
{
    auto db = GameContainer::getInstance()->getDatabase();
    GameObject* ship = new GameObject();


    auto ship_data = db->query("SELECT * FROM ships WHERE id = " + asstring(id))[0];
    auto ship_modules = db->query("SELECT * FROM ships_modules WHERE shipid = " + asstring(id));
    auto ship_unicontrol_map = db->query("SELECT * FROM ship_modules_united_controls_map");
    std::unordered_map<string, int> functionalityMap = {};

    for (int i = 0; i < ship_unicontrol_map.size(); i++) {
        auto link = ship_unicontrol_map[i];
        int func = asint(link["functionality"]);
        std::string name = link["module_link_name"];
        functionalityMap[name] = func;
    }

    auto model3d = readModel3d(asint(ship_data["modelid"]));

    auto drawableComponent = new AbsDrawableComponent(model3d, glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0));
    auto transformComponent = new Transformation3DComponent(asdouble(ship_data["mass"]), glm::dvec3(0.0));
    ship->addComponent(drawableComponent);
    ship->addComponent(transformComponent);

    ship->addComponent(new BatteryComponent(100000, 100000));
    
    for (int i = 0; i < ship_modules.size(); i++) {
        auto mod = ship_modules[i];
        glm::dvec3 pos = glm::dvec3(asdouble(mod["posx"]), asdouble(mod["posy"]), asdouble(mod["posz"]));
        glm::dquat rot = vec3toquat(glm::dvec3(asdouble(mod["dirx"]), asdouble(mod["diry"]), asdouble(mod["dirz"])), asdouble(mod["dirrot"]));
        auto mod_data = db->query("SELECT * FROM modules WHERE id = " + mod["moduleid"])[0];
        auto module_model3d = readModel3d(asint(mod_data["modelid"]));
        if (asint(mod_data["typeid"]) == MODULE_TYPE_ENGINE) {
            auto engine_data = db->query("SELECT * FROM engines WHERE id = " + mod_data["id"])[0];
            auto thrustgencomponent = new ThrustGeneratorComponent(module_model3d, pos, rot, asdouble(engine_data["power"]), asdouble(mod_data["wattage"]));
            thrustgencomponent->functionalityGroup = static_cast<ThrustGroup>(functionalityMap[mod["link_name"]]);
            ship->addComponent(thrustgencomponent);
        }
    }
    ship->addComponent(new ThrustControllerComponent());
    ship->addComponent(new ShipManualControlsComponent());
    
    return ship;
}
