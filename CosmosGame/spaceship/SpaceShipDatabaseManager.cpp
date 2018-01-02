#include "stdafx.h"
#include "SpaceShipDatabaseManager.h"
#include "SpaceShip.h"
#include "SpaceShipEngine.h"
#include "ShipEnginesUnitedController.h"
#include "../Model3d.h"
#include "../CosmosRenderer.h"
#include "SQLiteDatabase.h"


SpaceShipDatabaseManager::SpaceShipDatabaseManager(SQLiteDatabase* idb)
    : db(idb)
{
}


SpaceShipDatabaseManager::~SpaceShipDatabaseManager()
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

#define asstring(a) std::to_string(a)
#define asdouble(a) std::stod(a)
#define asint(a) std::stoi(a)
#define MODULE_TYPE_HYPERDRIVE 1
#define MODULE_TYPE_ENGINE 2

SpaceShip* SpaceShipDatabaseManager::readSpaceShip(int id)
{
    auto ship_data = db->query("SELECT * FROM ships WHERE id = " + asstring(id))[0];
    auto ship_modules = db->query("SELECT * FROM ships_modules WHERE shipid = " + asstring(id));
    auto ship_unicontrol_map = db->query("SELECT * FROM ship_modules_united_controls_map WHERE shipid = " + asstring(id));
     
    auto model3d = readModel3d(asint(ship_data["modelid"]));

    auto ship = new SpaceShip(model3d, glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0));

    for (int i = 0; i < ship_modules.size(); i++) {
        auto mod = ship_modules[i];
        glm::dvec3 pos = glm::dvec3(asdouble(mod["posx"]), asdouble(mod["posy"]), asdouble(mod["posz"]));
        glm::dquat rot = vec3toquat(glm::dvec3(asdouble(mod["dirx"]), asdouble(mod["diry"]), asdouble(mod["dirz"])), asdouble(mod["dirrot"]));
        auto mod_data = db->query("SELECT * FROM modules WHERE id = " + mod["moduleid"])[0];
        auto module_model3d = readModel3d(asint(mod_data["modelid"]));
        if (asint(mod_data["typeid"]) == MODULE_TYPE_ENGINE) {
            auto engine_data = db->query("SELECT * FROM engines WHERE id = " + mod_data["id"])[0];
            auto engine = new SpaceShipEngine(module_model3d, mod["link_name"], pos, rot, asdouble(engine_data["power"]), asdouble(mod_data["wattage"]));
            ship->modules.push_back(engine);
            engine->enable();
        }
    }
    /*
    its getting spaghetti in here
    functionalities:
    forward = 0
    backward = 1
    left = 2
    right = 3
    up = 4
    down = 5

    pitch_up = 6
    pitch_down = 7
    roll_left = 8
    roll_right = 9
    yaw_left = 10
    yaw_right = 11
    */
    for (int i = 0; i < ship_unicontrol_map.size(); i++) {
        auto link = ship_unicontrol_map[i];
        for (int g = 0; g < ship->modules.size(); g++) {
            auto mod = ship->modules[g];
            if (mod->instanceName == link["module_link_name"]) {
                int type = asint(link["functionality"]);
                switch (type) {
                case ShipEnginesUnitedControllerFunctionalities::forward:
                    ship->unitedController->addEnginesLinearNegativeZ({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::backward:
                    ship->unitedController->addEnginesLinearPositiveZ({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::left:
                    ship->unitedController->addEnginesLinearNegativeX({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::right:
                    ship->unitedController->addEnginesLinearPositiveX({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::up:
                    ship->unitedController->addEnginesLinearPositiveY({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::down:
                    ship->unitedController->addEnginesLinearNegativeY({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                    ///////////

                case ShipEnginesUnitedControllerFunctionalities::pitch_up:
                    ship->unitedController->addEnginesAngularNegativeX({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::pitch_down:
                    ship->unitedController->addEnginesAngularPositiveX({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::roll_left:
                    ship->unitedController->addEnginesAngularNegativeZ({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::roll_right:
                    ship->unitedController->addEnginesAngularPositiveZ({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::yaw_left:
                    ship->unitedController->addEnginesAngularNegativeY({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                case ShipEnginesUnitedControllerFunctionalities::yaw_right:
                    ship->unitedController->addEnginesAngularPositiveY({ static_cast<SpaceShipEngine*>(mod) });
                    break;
                }
            }
        }
    }
    return ship;
}

Model3d * SpaceShipDatabaseManager::readModel3d(int id)
{
    auto model3d_data = db->query("SELECT * FROM models3d WHERE id = " + asstring(id))[0];
    auto vulkan = CosmosRenderer::instance->vulkan;
    auto modeldset = CosmosRenderer::instance->modelMRTLayout;
    auto model3d = new Model3d(vulkan, modeldset, model3d_data["info3d_file"], model3d_data["albedo_image"], model3d_data["normal_image"],
        model3d_data["roughness_image"], model3d_data["metalness_image"], model3d_data["emission_idle_image"], model3d_data["emission_powered_image"]);
    return model3d;
}
