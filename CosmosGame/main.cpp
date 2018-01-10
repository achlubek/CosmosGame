#include "stdafx.h"
#include "CosmosRenderer.h"
#include "GalaxyGenerator.h"
#include "spaceship/SpaceShip.h"
#include "spaceship/SpaceShipModule.h"
#include "spaceship/SpaceShipDatabaseManager.h"
#include "spaceship/SpaceShipEngine.h"
#include "spaceship/SpaceShipHyperDrive.h"
#include "spaceship/SpaceShipAutopilot.h"
#include "spaceship/ShipEnginesUnitedController.h"
#include "AbsShipEnginesController.h"
#include "Model3d.h"
#include "Maneuvering6DOFShipEnginesController.h"
#include "CommandTerminal.h"
#include "physics/PhysicalWorld.h"
#include <algorithm>
#include "SQLiteDatabase.h"
#include "GameControls.h"
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

    INIReader* configreader = new INIReader("settings.ini");
    auto toolkit = new VulkanToolkit();
    toolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"));

    auto cosmosRenderer = new CosmosRenderer(toolkit, toolkit->windowWidth, toolkit->windowHeight);

    GalaxyGenerator* galaxy = cosmosRenderer->galaxy;
    //printf("gen");
    int64_t galaxyedge = 12490000000;
    int64_t galaxythickness = 1524900000;
    for (int i = 0; i < 10000; i++) {
        galaxy->generateStar(galaxyedge, galaxythickness, 1.0, i);
        cosmosRenderer->nearbyStars.push_back(galaxy->generateStarInfo(i));
    }
    for (int i = 0; i < cosmosRenderer->nearbyStars[1234].planets.size(); i++) {
        cosmosRenderer->nearbyStars[1234].planets[i].orbitSpeed = 0.0;
        //cosmosRenderer->nearbyStars[1234].planets[i].starDistance *= 0.01;
       // cosmosRenderer->nearbyStars[1234].planets[i].starDistance += 10000.0;
    }
    // printf("sea");
    // GeneratedStarInfo starinfo = galaxy->generateStarInfo(galaxy->findClosestStar(1, 1, 1));

    auto text = new UIText(cosmosRenderer->ui, 0.01, 0.01, UIColor(1, 1, 1, 1), Media::getPath("font.ttf"), 16, " ");
    auto closestPlanetText = new UIText(cosmosRenderer->ui, 0.01, 0.02, UIColor(1, 1, 1, 1), Media::getPath("font.ttf"), 16, " ");
    cosmosRenderer->ui->texts.push_back(text);
    cosmosRenderer->ui->texts.push_back(closestPlanetText);

    std::vector<UIText*> planetsLabels = {};
    for (int i = 0; i < 8; i++) {
        auto tx = new UIText(cosmosRenderer->ui, 0.01, 0.01, UIColor(1, 1, 1, 1), Media::getPath("font.ttf"), 16, " ");
        tx->horizontalAlignment = UIText::HorizontalAlignment::center;
        tx->verticalAlignment = UIText::VerticalAlignment::center;
        planetsLabels.push_back(tx);
        cosmosRenderer->ui->texts.push_back(tx);
    } 

    UIBitmap* marker = new UIBitmap(cosmosRenderer->ui, 0.0, 0.0, 0.15, 0.15, cosmosRenderer->assets.loadTextureFile("crosshair_helper.png"), UIColor(7,7,7,7));
    cosmosRenderer->ui->bitmaps.push_back(marker);
    marker->horizontalAlignment = UIBitmap::HorizontalAlignment::center;
    marker->verticalAlignment = UIBitmap::VerticalAlignment::center;

    auto camera = new Camera();
    float cameraFov = 80.0;
    camera->createProjectionPerspective(80.0f, (float)toolkit->windowWidth / (float)toolkit->windowHeight, 0.01f, 9000000.0f);
    glm::quat cameraLookatOffset = glm::quat(1.0, 0.0, 0.0, 0.0);
    // glm::dvec3 observerPosition;
    
    Mouse* mouse = new Mouse(toolkit->window);
    Keyboard* keyboard = new Keyboard(toolkit->window);

    GameControls controls = GameControls(keyboard, "controls.ini");

    Joystick* joystick = new Joystick(toolkit->window);
    CommandTerminal* terminal = new CommandTerminal(cosmosRenderer->ui, keyboard);
    float pitch = 0.0;
    float yaw = 0.0;
    int lastcx = 0, lastcy = 0;
    int frames = 0;
    double lastTime = 0.0;
    double lastRawTime = 0.0;
    float speedmultiplier = 1.0;
    float stabilizerotation = 1.0;

    bool flightHelperEnabled = false;

    int helper_orientationMode_dampingAbsolute = 0;
    int helper_orientationMode_matchTarget = 1;
    int helper_orientationMode_align = 2;
    glm::dquat orientationQuatTarget;
    glm::dvec3 orientationAlignTarget;

    int helper_positionMode_matchClosestBody = 0;
    int helper_positionMode_enterBodyOrbit = 1;
    int helper_positionMode_holdStillPositionOverSurface = 2;

    double helper_orbitRadius;

    int helper_orientationMode = helper_orientationMode_align;
    int helper_positionMode = helper_positionMode_matchClosestBody;

    mouse->onMouseDown.add([&](int key) {
        if (key == GLFW_MOUSE_BUTTON_MIDDLE) {
            cameraLookatOffset = glm::quat(1.0, 0.0, 0.0, 0.0);
            yaw = 0;
            pitch = 0;
        }
    });

    mouse->onMouseScroll.add([&](int yoffset) {
        cameraFov -= (float)yoffset * 1.1f;
        cameraFov = glm::clamp(cameraFov, 1.0f, 110.0f);
        camera->createProjectionPerspective(cameraFov, (float)toolkit->windowWidth / (float)toolkit->windowHeight, 0.01f, 9000000.0f);
    });
    //glm::vec3 spaceshipLinearVelocity = glm::vec3(0.0);
    //glm::vec3 spaceshipAngularVelocity = glm::vec3(0.0);
    //glm::dvec3 spaceshipPosition = glm::dvec3(0.0);
    //glm::quat spaceshipOrientation = glm::quat(1.0, 0.0, 0.0, 0.0);

    PhysicalWorld* pworld = new PhysicalWorld();
    PhysicalEntity* player = new PhysicalEntity(10.0, glm::dvec3(0.0));
    Model3d* shipModel = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "spaceship2d.ini");
    //SpaceShip* ship = new SpaceShip(placeholdercolshape, shipModel, cosmosRenderer->nearbyStars[9999].planets[0].getPosition(100.0) * cosmosRenderer->scale + glm::dvec3(10000.0, 0.0, 0.0), glm::dquat(1.0, 0.0, 0.0, 0.0));

    SQLiteDatabase* db = new SQLiteDatabase(Media::getPath("gamedata.db")); 
    SpaceShipDatabaseManager* dbmngr = new SpaceShipDatabaseManager(db);
    auto ship = dbmngr->readSpaceShip(1);

    player->setPosition(ship->getPosition());

    //pworld->entities.push_back(player);
    pworld->entities.push_back(ship);

    ship->mainSeat = player;


    keyboard->onKeyPress.add([&](int key) {
        if (key == GLFW_KEY_T) {
            if (!terminal->isInputEnabled()) {
                terminal->enableInput();
            }
        }
        if (key == GLFW_KEY_ESCAPE) {
            if (terminal->isInputEnabled()) {
                terminal->disableInput();
            }
        }
        if (terminal->isInputEnabled()) return;
        if (key == GLFW_KEY_O) {
            // flightHelperEnabled = !flightHelperEnabled;
            ship->setLinearVelocity(glm::dvec3(0.0));
            ship->setAngularVelocity(glm::dvec3(0.0));
        }
        if (key == GLFW_KEY_F1) {
            speedmultiplier = 0.00015;
        }
        if (key == GLFW_KEY_F2) {
            speedmultiplier = 0.005;
        }
        if (key == GLFW_KEY_F3) {
            speedmultiplier = 0.1;
        }
        if (key == GLFW_KEY_F4) {
            speedmultiplier = 1.0;
        }

        if (key == GLFW_KEY_F5) {
            stabilizerotation = 0.0;
        }
        if (key == GLFW_KEY_F6) {
            stabilizerotation = 0.25;
        }
        if (key == GLFW_KEY_F7) {
            stabilizerotation = 0.5;
        }
        if (key == GLFW_KEY_F8) {
            stabilizerotation = 1.0;
        }
        if (key == GLFW_KEY_PAUSE) {
            cosmosRenderer->recompileShaders(true);
        }
    });
    /*
    // try sql
    db.query("create table test2 ('id' integer, 'name' text);");
    db.query("insert into test2 (id, name) values (123, 'def');");
    db.query("insert into test2 (id, name) values (666, 'asd');");
    auto res = db.query("select * from test2");
    for (int i = 0; i < res.size(); i++) {
        std::cout << "row" << i << '\n';
        for (auto p : res[i])
        {
            std::cout << p.first << " = " << p.second << '\n';
        }
    }*/
    /*
    Model3d* smallEngineModel1 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel2 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel3 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel4 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");

    Model3d* smallEngineModel5 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel6 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel7 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel8 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");

    Model3d* smallEngineModel9 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel10 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel11 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");
    Model3d* smallEngineModel12 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "small_engine.ini");


    Model3d* largeEngineModel1 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "large_engine.ini");
    Model3d* largeEngineModel2 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "large_engine.ini");
    Model3d* hyperEngineModel1 = new Model3d(toolkit, cosmosRenderer->modelMRTLayout, "hyperdrive.ini");

    SpaceShipHyperDrive* hyperdrive_engine = new SpaceShipHyperDrive(hyperEngineModel1, glm::dvec3(0.0, 0.0, 10.0), vec3toquat(glm::dvec3(0.0, 0.0, 1.0)), 2500000.19, 0.1);

    SpaceShipEngine* forward_engine = new SpaceShipEngine(largeEngineModel1, glm::dvec3(0.0, 0.0, 10.0), vec3toquat(glm::dvec3(0.0, 0.0, 1.0)), 100000.19, 0.1);
    SpaceShipEngine* backward_engine = new SpaceShipEngine(largeEngineModel2, glm::dvec3(0.0, 0.0, -10.0), vec3toquat(glm::dvec3(0.0, 0.0, -1.0)), 100000.19, 0.01);
    */
    /*
             rX |   / rY
                |  /
                | /
     rZ         |/        rZ
     -----------+-----------
               /|
              / |
             /  |
         rY /   | rX

         total 12 engines
         negXUP
         negXDOWN
         posXUP
         posXDOWN

         negYUP
         negYDOWN
         posYUP
         posYDOWN

         negZUP
         negZDOWN
         posZUP
         posZDOWN
    *//*
    double helpersforce = 100.5;
    SpaceShipEngine* negXUP = new SpaceShipEngine(smallEngineModel1, glm::dvec3(-10.0, 0.0, 0.0), vec3toquat(glm::dvec3(0.0, 1.0, 0.0)), helpersforce, 0.1);
    SpaceShipEngine* negXDOWN = new SpaceShipEngine(smallEngineModel2, glm::dvec3(-10.0, 0.0, 0.0), vec3toquat(glm::dvec3(0.0, -1.0, 0.0)), helpersforce, 0.1);
    SpaceShipEngine* posXUP = new SpaceShipEngine(smallEngineModel3, glm::dvec3(10.0, 0.0, 0.0), vec3toquat(glm::dvec3(0.0, 1.0, 0.0)), helpersforce, 0.1);
    SpaceShipEngine* posXDOWN = new SpaceShipEngine(smallEngineModel4, glm::dvec3(10.0, 0.0, 0.0), vec3toquat(glm::dvec3(0.0, -1.0, 0.0)), helpersforce, 0.1);

    SpaceShipEngine* negYFORWARD = new SpaceShipEngine(smallEngineModel5, glm::dvec3(0.0, -10.0, 0.0), vec3toquat(glm::dvec3(0.0, 0.0, 1.0)), helpersforce, 0.1);
    SpaceShipEngine* negYBACKWARD = new SpaceShipEngine(smallEngineModel6, glm::dvec3(0.0, -10.0, 0.0), vec3toquat(glm::dvec3(0.0, 0.0, -1.0)), helpersforce, 0.1);
    SpaceShipEngine* posYFORWARD = new SpaceShipEngine(smallEngineModel7, glm::dvec3(0.0, 10.0, 0.0), vec3toquat(glm::dvec3(0.0, 0.0, 1.0)), helpersforce, 0.1);
    SpaceShipEngine* posYBACKWARD = new SpaceShipEngine(smallEngineModel8, glm::dvec3(0.0, 10.0, 0.0), vec3toquat(glm::dvec3(0.0, 0.0, -1.0)), helpersforce, 0.1);

    SpaceShipEngine* negZLEFT = new SpaceShipEngine(smallEngineModel9, glm::dvec3(0.0, 0.0, -10.0), vec3toquat(glm::dvec3(-1.0, 0.0, 0.0)), helpersforce, 0.1);
    SpaceShipEngine* negZRIGHT = new SpaceShipEngine(smallEngineModel10, glm::dvec3(0.0, 0.0, -10.0), vec3toquat(glm::dvec3(1.0, 0.0, 0.0)), helpersforce, 0.1);
    SpaceShipEngine* posZLEFT = new SpaceShipEngine(smallEngineModel11, glm::dvec3(0.0, 0.0, 10.0), vec3toquat(glm::dvec3(-1.0, 0.0, 0.0)), helpersforce, 0.1);
    SpaceShipEngine* posZRIGHT = new SpaceShipEngine(smallEngineModel12, glm::dvec3(0.0, 0.0, 10.0), vec3toquat(glm::dvec3(1.0, 0.0, 0.0)), helpersforce, 0.1);

    Maneuvering6DOFShipEnginesController enginesController = Maneuvering6DOFShipEnginesController();
    enginesController.negXUP = negXUP;
    enginesController.negXDOWN = negXDOWN;
    enginesController.posXUP = posXUP;
    enginesController.posXDOWN = posXDOWN;
    enginesController.negYFORWARD = negYFORWARD;
    enginesController.negYBACKWARD = negYBACKWARD;
    enginesController.posYFORWARD = posYFORWARD;
    enginesController.posYBACKWARD = posYBACKWARD;
    enginesController.negZLEFT = negZLEFT;
    enginesController.negZRIGHT = negZRIGHT;
    enginesController.posZLEFT = posZLEFT;
    enginesController.posZRIGHT = posZRIGHT;

    SpaceShipAutopilot pilot = SpaceShipAutopilot();
    pilot.controller = &enginesController;

    ship->modules.push_back(hyperdrive_engine);
    ship->modules.push_back(forward_engine);
    ship->modules.push_back(backward_engine);

    ship->modules.push_back(negXUP);
    ship->modules.push_back(negXDOWN);
    ship->modules.push_back(posXUP);
    ship->modules.push_back(posXDOWN);

    ship->modules.push_back(negYFORWARD);
    ship->modules.push_back(negYBACKWARD);
    ship->modules.push_back(posYFORWARD);
    ship->modules.push_back(posYBACKWARD);

    ship->modules.push_back(negZLEFT);
    ship->modules.push_back(negZRIGHT);
    ship->modules.push_back(posZLEFT);
    ship->modules.push_back(posZRIGHT);


    for (int i = 0; i < ship->modules.size(); i++) {
        ship->modules[i]->enable();
    }
    */
    cosmosRenderer->ships.push_back(ship);

    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStars();
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->updateNearestStar(ship->getPosition());
            cosmosRenderer->updateGravity(ship->getPosition());

        }
    });
    background1.detach();
    std::thread background2 = std::thread([&]() {
        auto last = std::chrono::high_resolution_clock::now();
        while (true) {
            auto now = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - last).count();
            pworld->stepEmulation(elapsed);
            last = now;
            std::this_thread::sleep_for(1ms);
        }
    });
    background2.detach();

    /*
    transparency and fragment ordering :
    The only sane way is to make it correct ALWAYS
    struct afragmment{
        float depth
        float alpha
        vec3 color
    }
    make a table with 10 these structures
    then render shit
    then order it and thats it done thank you bye
    */
    /*
    terminal->onSendText.add([&](std::string s) {
        if (s[0] == '/') {
            std::string cmd = s.substr(1);
            std::vector<std::string> words = {};
            splitBySpaces(words, cmd);
            if (cmd == "help") {
                terminal->printMessage(UIColor(0.1, 1.0, 0.1, 1.0), "*** Hope you enjoy.");
                return;
            }
            if (words.size() == 3 && words[0] == "warp_to_planet") {
                std::thread hyperdrive_proc([&]() {
                    glm::dvec3 target = cosmosRenderer->nearbyStars[std::stoi(words[1])].planets[std::stoi(words[2])].getPosition(0.0) * cosmosRenderer->scale;
                    pilot.setAngularMaxSpeed(1.0);
                    pilot.setLinearMaxSpeed(100.0);
                    pilot.setPositionCorrectionStrength(0.135);
                    // terminal->printMessage(UIColor(0.1, 1.0, 0.1, 1.0), "*** Braking...");
                    enginesController.setEnginesPower(0.1);
                    enginesController.setReferenceFrameVelocity(glm::dvec3(0.0));
                    enginesController.setTargetAngularVelocity(glm::dvec3(0.0));
                    enginesController.setTargetLinearVelocity(glm::dvec3(0.0));
                    auto dstorient = glm::normalize(target - ship->getPosition());
                    while (true) {
                        pilot.setTargetPosition(ship->getPosition() + ship->getLinearVelocity() * -100.0);
                        if (glm::length(ship->getLinearVelocity()) < 0.01 && glm::length(ship->getAngularVelocity()) < 0.1) break;
                        pilot.update(ship);
                        pilot.getTargetOrientation(dstorient);
                        enginesController.update(ship);
                    }
                    // terminal->printMessage(UIColor(0.1, 1.0, 0.1, 1.0), "*** Aligning...");
                    pilot.setTargetPosition(ship->getPosition());
                    pilot.getTargetOrientation(dstorient);
                    while (true) {
                        if (glm::length(ship->getLinearVelocity()) < 0.01 && glm::length(dstorient - ship->getOrientation() * glm::dvec3(0.0, 0.0, -1.0)) < 0.01
                            && glm::length(ship->getAngularVelocity()) < 0.001) break;
                        pilot.update(ship);
                        enginesController.update(ship);
                    }
                    //  terminal->printMessage(UIColor(0.1, 1.0, 0.1, 1.0), "*** Enabling hyperdrive...");
                    enginesController.setEnginesPower(1.0);
                    hyperdrive_engine->setPowerPercentage(1.0);
                    double targetdist = glm::length(target - ship->getPosition());
                    glm::dvec3 start = ship->getPosition();
                    while (true) {
                        double dst = glm::length(start - ship->getPosition());
                        double dst2 = glm::length(target - ship->getPosition());
                        pilot.setTargetPosition(ship->getPosition() + ship->getLinearVelocity() * -100.0);
                        dstorient = glm::normalize(target - ship->getPosition());
                        pilot.getTargetOrientation(dstorient);
                        pilot.update(ship);
                        enginesController.update(ship);
                        hyperdrive_engine->setPowerPercentage((dst2 - 3000.0) / (5400000.0));
                        printf("%f %f %f %f\n", hyperdrive_engine->getPowerPercentage(), targetdist, dst, dst2);
                        if (dst > targetdist - 3000.0) {
                            hyperdrive_engine->setPowerPercentage(0.0);
                            break;
                        }
                    }
                    // terminal->printMessage(UIColor(0.1, 1.0, 0.1, 1.0), "*** Done.");
                });
                hyperdrive_proc.detach();
                return;
            }
            if (words.size() == 3 && words[0] == "tp") {
                auto targetplanet = cosmosRenderer->nearbyStars[std::stoi(words[1])].planets[std::stoi(words[2])];
                glm::dvec3 target = targetplanet.getPosition(0.0) * cosmosRenderer->scale;
                enginesController.setReferenceFrameVelocity(glm::dvec3(0.0));
                enginesController.setTargetAngularVelocity(glm::dvec3(0.0));
                enginesController.setTargetLinearVelocity(glm::dvec3(0.0));
                ship->setPosition(target - ship->getOrientation() * glm::dvec3(0.0, 0.0, targetplanet.radius * 3.0 * cosmosRenderer->scale));
               // ship->setPosition(ship->getPosition() - ship->getOrientation() * glm::dvec3(0.0, 0.0, 11111.0));
            }
            terminal->printMessage(UIColor(1.0, 1.0, 0.0, 1.0), "* Invalid command.");
            return;
        }
        terminal->printMessage(UIColor(0.5, 0.5, 0.5, 1.0), s);
    });*/


#define multiplyscale(a,b) (std::pow(a, b))
    while (!toolkit->shouldCloseWindow()) {
        frames++;
        double time = glfwGetTime();
        double nowtime = floor(time);
        if (nowtime != lastTime) {
            printf("FPS %d\n", frames);
            frames = 0;
        }
        double elapsed_x100 = (float)(100.0 * (time - lastRawTime));
        double elapsed = (float)((time - lastRawTime));
        lastRawTime = time;
        lastTime = nowtime;
         
        double speed = 1.1f * elapsed_x100;
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            speed *= 0.1f;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            speed *= 0.1f;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            speed *= 10;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 730.1f;
        }
        glm::dvec3 dw = glm::vec3(0);
        double w = 0.0;
        if (keyboard->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
            dw +=  glm::vec3(0, 0, -1);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
            dw += glm::vec3(0, 0, 1);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
            dw +=  glm::vec3(-1, 0, 0);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
            dw += glm::vec3(1, 0, 0);
            w += 1.0;
        }
        glm::dvec3 a = dw / w;
        glm::dvec3 dir = w > 0.0 ? a : dw;
        glm::dvec3 newpos = player->getPosition();
        newpos += length(dir) > 0.0 ? (normalize(dir) * speed) : (glm::dvec3(0.0));

        glm::dvec2 cursor;
        auto tup = mouse->getCursorPosition();
        cursor.x = get<0>(tup);
        cursor.y = get<1>(tup);

        float dx = (float)(lastcx - cursor.x);
        float dy = (float)(lastcy - cursor.y);

        lastcx = cursor.x;
        lastcy = cursor.y;
        if (mouse->isButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
            mouse->setCursorMode(GLFW_CURSOR_HIDDEN);
            yaw += dy * 0.2f;
            pitch += dx * 0.2f;
            if (yaw < -90.0) yaw = -90;
            if (yaw > 90.0) yaw = 90;
            if (pitch < -360.0f) pitch += 360.0f;
            if (pitch > 360.0f) pitch -= 360.0f;
            glm::quat newrot = glm::angleAxis(deg2rad(pitch), glm::vec3(0, 1, 0)) * glm::angleAxis(deg2rad(yaw), glm::vec3(1, 0, 0));
            cameraLookatOffset = glm::slerp(newrot, cameraLookatOffset, 0.9f);
        }
        else {
            mouse->setCursorMode(GLFW_CURSOR_NORMAL);
        }
        //   camera->transformation->setOrientation(glm::slerp(newrot, camera->transformation->getOrientation(), 0.9f));
        //   player->setOrientation(newrot);
        // player->applyImpulse(glm::dvec3(0.0, 0.0, -1.0), length(dir) > 0.0 ? (normalize(dir) * speed) : (glm::dvec3(0.0)));
        /*glm::dvec3 outposhit;
        bool hits = ship->hitRayPosition(player->getPosition(), glm::normalize(dir), outposhit);
        if (!hits || glm::distance(outposhit, player->getPosition()) > glm::distance(newpos, player->getPosition())) {
            player->setPosition(newpos);
        }*/
        /*
        if (!terminal->isInputEnabled()) {
            if (flightHelperEnabled) {
                //debug
                orientationAlignTarget = glm::normalize(cosmosRenderer->closestBodyPosition - ship->getPosition());
                
                glm::dvec3 angularThrust;
                glm::dvec3 targetAngularSpeed;
                if (helper_orientationMode == helper_orientationMode_dampingAbsolute) {
                    targetAngularSpeed = glm::dvec3(0.0, 0.0, 0.0);


                }
                else if (helper_orientationMode == helper_orientationMode_align) {

                    pilot.getTargetOrientation(orientationAlignTarget);


                }
                glm::dvec3 targetVelocity = glm::dvec3();
                if (helper_positionMode == helper_positionMode_matchClosestBody) {
                    targetVelocity = glm::dvec3(0.0);
                    pilot.setTargetPosition(ship->getPosition());
                }
                if (helper_positionMode == helper_positionMode_enterBodyOrbit) {
                    glm::dvec3 center = cosmosRenderer->closestBodyPosition;
                    float dst = glm::distance(center, ship->getPosition());
                    double rad = glm::distance(center, cosmosRenderer->closestSurfacePosition);
                    double orbitradius = 20.0;
                    printf("R  %.6f \n", orbitradius);
                    double targettangentialspeed = cosmosRenderer->scale * 0.0 *  cosmosRenderer->galaxy->calculateOrbitVelocity(dst, galaxy->calculateMass(rad / cosmosRenderer->scale));
                    targetVelocity = cosmosRenderer->closestObjectLinearAbsoluteSpeed;

                    glm::dvec3 dir = glm::normalize(cosmosRenderer->closestSurfacePosition - cosmosRenderer->closestBodyPosition);
                    glm::dvec3 dir2 = glm::normalize((cosmosRenderer->closestSurfacePosition + glm::normalize(ship->getLinearVelocity() + glm::dvec3(0.0, 0.001, 0.0)) - cosmosRenderer->closestBodyPosition) * 0.1);
                    glm::dvec3 heightcorrection = dir * (dst - orbitradius) * -0.1;
                    glm::dvec3 speedcorrection = glm::normalize(dir2 - dir) * targettangentialspeed;
                    targetVelocity = cosmosRenderer->closestObjectLinearAbsoluteSpeed + speedcorrection + heightcorrection;
                    pilot.setTargetPosition(cosmosRenderer->closestSurfacePosition + dir * orbitradius);
                }
                pilot.setAngularMaxSpeed(10.0);
                pilot.setLinearMaxSpeed(100.0);
                pilot.setPositionCorrectionStrength(0.135);
                //pilot.update(ship);
                enginesController.setTargetLinearVelocity(targetVelocity);
                enginesController.setEnginesPower(1.0);
                enginesController.setReferenceFrameVelocity(cosmosRenderer->closestObjectLinearAbsoluteSpeed);
                enginesController.update(ship);

            }
            else {
                // manual sterring
                forward_engine->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS ? speedmultiplier : 0.0);
                backward_engine->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS ? speedmultiplier : 0.0);

                //X
                float rotsped = 0.01;
                negYFORWARD->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_J) == GLFW_PRESS ? rotsped : 0.0);
                negYBACKWARD->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_U) == GLFW_PRESS ? rotsped : 0.0);
                posYBACKWARD->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_J) == GLFW_PRESS ? rotsped : 0.0);
                posYFORWARD->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_U) == GLFW_PRESS ? rotsped : 0.0);

                //Y
                negXUP->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS ? rotsped : 0.0);
                negXDOWN->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS ? rotsped : 0.0);
                posXDOWN->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS ? rotsped : 0.0);
                posXUP->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS ? rotsped : 0.0);

                //Z
                negZLEFT->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_K) == GLFW_PRESS ? rotsped : 0.0);
                negZRIGHT->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_H) == GLFW_PRESS ? rotsped : 0.0);
                posZRIGHT->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_K) == GLFW_PRESS ? rotsped : 0.0);
                posZLEFT->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_H) == GLFW_PRESS ? rotsped : 0.0);


                // hyperdrive_engine->setPowerPercentage(keyboard->getKeyStatus(GLFW_KEY_P) == GLFW_PRESS ? 1.0 : 0.0;
            }
            /*
            if (keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(0, 0, 1);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_R) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(0, 1, 0);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_F) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(0, -1, 0);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_Q) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(-1, 0, 0);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_E) == GLFW_PRESS) {
                spaceshipLinearVelocity += speedmultiplier * elapsed_x100 * rotmat * glm::vec3(1, 0, 0);
            }
            if (keyboard->getKeyStatus(GLFW_KEY_SPACE) == GLFW_PRESS) {
                spaceshipLinearVelocity = glm::vec3(cosmosRenderer->closestObjectLinearAbsoluteSpeed * 0.01);
            }
            spaceshipAngularVelocity *= multiplyscale(1.0f - stabilizerotation * 0.1f, elapsed_x100);
            if (keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
                spaceshipAngularVelocity.z += elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
                spaceshipAngularVelocity.z -= elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_U) == GLFW_PRESS) {
                spaceshipAngularVelocity.x -= elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_J) == GLFW_PRESS) {
                spaceshipAngularVelocity.x += elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_H) == GLFW_PRESS) {
                spaceshipAngularVelocity.y += elapsed_x100 * 0.0005;
            }
            if (keyboard->getKeyStatus(GLFW_KEY_K) == GLFW_PRESS) {
                spaceshipAngularVelocity.y -= elapsed_x100 * 0.0005;
            }*/
            /*
            if (joystick->isPresent(0)) {
                auto axes = joystick->getAxes(0);
                spaceshipLinearVelocity += 70.0f * ((-axes[3])) * elapsed_x100 * rotmat * glm::vec3(0, 0, -1);
                spaceshipAngularVelocity.x += elapsed_x100 * 0.001 * -axes[1];
                spaceshipAngularVelocity.y += elapsed_x100 * 0.001 * -axes[2];
                spaceshipAngularVelocity.z += elapsed_x100 * 0.001 * -axes[0];
            }

        }
        */
        int cnt = cosmosRenderer->nearestStarSystems.size() > 0 ? cosmosRenderer->nearestStarSystems[0].planets.size() : 0;
        for (int i = 0; i < cnt; i++) {
            glm::dvec3 pos = cosmosRenderer->nearestStarSystems[0].planets[i].getPosition(0.0) * cosmosRenderer->scale - player->getPosition();
            auto uv = camera->projectToScreen(pos);
            planetsLabels[i]->x = uv.x;
            planetsLabels[i]->y = uv.y;
            planetsLabels[i]->updateText(cosmosRenderer->nearestStarSystems[0].planets[i].getName());
        }
        for (int i = cnt; i < planetsLabels.size(); i++) {
            planetsLabels[i]->x = -1;
            planetsLabels[i]->y = -1;
        }

        auto linear_thrust_controls = glm::dvec3(
            controls.readAxisValue("united_controller_manual_linear_left_right_axis"),
            controls.readAxisValue("united_controller_manual_linear_up_down_axis"),
            controls.readAxisValue("united_controller_manual_linear_forward_backward_axis")
        );
        ship->unitedController->setLinearThrust(
            linear_thrust_controls
        );

        camera->transformation->setOrientation(player->getOrientation() * glm::dquat(cameraLookatOffset));
       // ship->applyGravity(cosmosRenderer->lastGravity * elapsed * (keyboard->getKeyStatus(GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS ? 100.0f : 1.0f));
       // ship->stepEmulation(elapsed);
        //camera->transformation->setOrientation(ship->getOrientation());
        cosmosRenderer->updateCameraBuffer(camera, player->getPosition());
        cosmosRenderer->updatePlanetsAndMoon(ship->getPosition());
        cosmosRenderer->draw();
        auto slp = ship->getLinearVelocity();
        //auto slp2 = ship->closestSurface(player->getPosition());
        //auto uv = camera->projectToScreen(slp2 - player->getPosition());
       // marker->x = uv.x;
      //  marker->y = uv.y;
       // slp2 = player->getPosition() - slp2;
        text->updateText("Distance to surface:" + std::to_string(cosmosRenderer->closestSurfaceDistance) + "| " + std::to_string(cosmosRenderer->closestObjectLinearAbsoluteSpeed.x)
            + " , " + std::to_string(cosmosRenderer->closestObjectLinearAbsoluteSpeed.y) + " , " + std::to_string(cosmosRenderer->closestObjectLinearAbsoluteSpeed.z)
            + "| " + std::to_string(slp.x)
            + " , " + std::to_string(slp.y)
            + " , " + std::to_string(slp.z)
            + "| " + std::to_string(linear_thrust_controls.x)
            + " , " + std::to_string(linear_thrust_controls.y)
            + " , " + std::to_string(linear_thrust_controls.z));
        closestPlanetText->updateText("Closest planet:" + cosmosRenderer->nearestPlanet.getName());
        toolkit->poolEvents();
    }

    cosmosRenderer->unmapBuffers();
    return 0;
}

