#include "stdafx.h"

int main(int argc, char** argv)
{ 
    uint64_t targetStar = 0;
    uint64_t targetPlanet = 0;
    uint64_t targetMoon = 0;

    if (argc == 0) {
        printf("Usage: editor.exe star [planet] [moon]\n");
        return 0;
    }

    if (argc > 1) {
        targetStar = std::stol(argv[1]);
    }

    if (argc > 2) {
        targetPlanet = std::stol(argv[2]);
    }

    if (argc > 3) {
        targetMoon = std::stol(argv[3]);
    }

    Media::loadFileMap("../../media");
    Media::loadFileMap("../../shaders");

    auto database = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(database);

    glm::dvec3 center = glm::dvec3(0.0);
    double dist = 0.0;

    GeneratedStarInfo star = galaxy->getAllStars()[targetStar - 1];
    center = star.getPosition(0);
    dist = star.radius;
    if (targetPlanet > 0) {
        galaxy->update(star.getPosition(0));
        GeneratedPlanetInfo planet = galaxy->getClosestStarPlanets()[targetPlanet - 1];
        center = planet.getPosition(0);
        dist = planet.radius;
        if (targetMoon > 0) {
            galaxy->update(planet.getPosition(0));
            GeneratedMoonInfo moon = galaxy->getClosestPlanetMoons()[targetMoon - 1];
            center = moon.getPosition(0);
            dist = moon.radius;
        }
    }
    float mindist = dist+1.0;
    dist *= 2.0;

    INIReader* configreader = new INIReader("settings.ini");
    auto vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"));


    UIRenderer* ui = new UIRenderer(vulkanToolkit, configreader->geti("window_width"), configreader->geti("window_height"));

    auto cosmosRenderer = new CosmosRenderer(vulkanToolkit, galaxy, ui->outputImage, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);
    
    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    keyboard->onKeyPress.add([&](int key) {
        if (key == GLFW_KEY_PAUSE) cosmosRenderer->recompileShaders(true);
    });


    Camera * cam = new Camera();
    cam->createProjectionPerspective(90.0f, (float)configreader->geti("window_width") / (float)configreader->geti("window_height"), 0.01f, 9000000.0f);

    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStarsBuffer();

    double xrot = 0.0;
    double yrot = 0.0;

    double Rxrot = 0.0;
    double Ryrot = 0.0;

    int frames = 0;
    double lastTime = 0.0;
    double lastRawTime = 0.0;
    while (!vulkanToolkit->shouldCloseWindow()) {
        frames++;
        double time = glfwGetTime();
        double nowtime = floor(time);
        if (nowtime != lastTime) {
            printf("FPS %d\n", frames);
            frames = 0;
        }
        double elapsed = (float)((time - lastRawTime));
        lastRawTime = time;
        lastTime = nowtime;

        float orbitSpeed = elapsed;
        float panSpeed = elapsed;
        float zoomSpeed = elapsed;
        if (keyboard->getKeyStatus(GLFW_KEY_UP) == GLFW_PRESS) {
            xrot -= orbitSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_DOWN) == GLFW_PRESS) {
            xrot += orbitSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT) == GLFW_PRESS) {
            yrot -= orbitSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_RIGHT) == GLFW_PRESS) {
            yrot += orbitSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
            Rxrot -= panSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
            Rxrot += panSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
            Ryrot -= panSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
            Ryrot += panSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            dist = dist * 0.95 + mindist * 0.05;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            dist += zoomSpeed * 5000.0;
        }


        auto rotxmat = glm::angleAxis(xrot, glm::dvec3(1.0, 0.0, 0.0));
        auto rotymat = glm::angleAxis(yrot, glm::dvec3(0.0, 1.0, 0.0));

        auto Rrotxmat = glm::angleAxis(-Rxrot, glm::dvec3(1.0, 0.0, 0.0));
        auto Rrotymat = glm::angleAxis(-Ryrot, glm::dvec3(0.0, 1.0, 0.0));

        glm::dvec3 dir = rotymat * (rotxmat * glm::dvec3(0.0, 0.0, -1.0));
        cam->transformation->setOrientation(rotymat * rotxmat * Rrotymat * Rrotxmat);

        ui->draw();
        cosmosRenderer->updateCameraBuffer(cam, center - dir * dist);
        cosmosRenderer->draw();
        vulkanToolkit->poolEvents();
    }

    return 0;
}

