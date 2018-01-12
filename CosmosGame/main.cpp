#include "stdafx.h"
#include "CosmosRenderer.h"
#include "GalaxyGenerator.h"
#include "SpaceShip.h"
#include "SpaceShipModule.h"
#include "SpaceShipDatabaseManager.h"
#include "SpaceShipEngine.h"
#include "SpaceShipHyperDrive.h"
#include "SpaceShipAutopilot.h"
#include "ShipEnginesUnitedController.h"
#include "AbsShipEnginesController.h"
#include "Model3d.h"
#include "Maneuvering6DOFShipEnginesController.h"
#include "CommandTerminal.h"
#include "PhysicalWorld.h"
#include <algorithm>
#include "SQLiteDatabase.h"
#include "GameControls.h"
#include "GameContainer.h"
using namespace std;
void splitBySpaces(vector<string>& output, string src)
{
    int i = 0, d = 0;
    while (i < src.size()) {
        if (src[i] == ' ') {
            output.push_back(src.substr(d, i - d));
            d = i;
            while (src[i++] == ' ')  d++;
        }
        else {
            i++;
        }
    }
    if (i == src.size() && d < i) {
        output.push_back(src.substr(d, i));
    }
}
/*
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
*/
int main()
{ 
    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    GameContainer * game = new GameContainer();
    game->startGameLoops();
    return 0;

}

