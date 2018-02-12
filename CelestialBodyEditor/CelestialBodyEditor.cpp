#include "stdafx.h"
#include "SceneProvider.h"
#include "TimeProvider.h"

class EmptySceneProvider : public SceneProvider
{
    virtual void drawDrawableObjects(VulkanRenderStage * stage, VulkanDescriptorSet * set) override
    {
    }
};

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


    INIReader* configreader = new INIReader("settings.ini");
    auto vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"));

    UIRenderer* ui = new UIRenderer(vulkanToolkit, configreader->geti("window_width"), configreader->geti("window_height"));

    auto timeProvider = new TimeProvider();
    auto cosmosRenderer = new CosmosRenderer(vulkanToolkit, timeProvider, new EmptySceneProvider(), galaxy, ui->outputImage, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    glm::dvec3 center = glm::dvec3(0.0);
    double dist = 0.0;

    GeneratedStarInfo star = galaxy->getAllStars()[targetStar - 1];
    center = star.getPosition(0);
    dist = star.radius;
    CelestialBody targetBody = CelestialBody();
    if (targetPlanet > 0) {
        galaxy->update(star.getPosition(0));
        auto planet = galaxy->getClosestStarPlanets()[targetPlanet - 1];
        center = planet.getPosition(0);
        dist = planet.radius;
        targetBody = planet;
        if (targetMoon > 0) {
            galaxy->update(planet.getPosition(0));
            auto moon = galaxy->getClosestPlanetMoons()[targetMoon - 1];
            center = moon.getPosition(0);
            dist = moon.radius;
            targetBody = moon;
        }
    }
    float mindist = dist + 0.001;
    dist *= 2.0;


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
        double elapsed = (double)((time - lastRawTime));
        timeProvider->update(elapsed);
        lastRawTime = time;
        lastTime = nowtime;

        center = targetBody.getPosition(timeProvider->getTime());

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

