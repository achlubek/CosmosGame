#include "stdafx.h"
#include "UtilitiesLib.h"
#include "GalaxyLib.h"
#include "CosmosRendererLib.h"
#include "SQLiteDatabase.h"
#include "VEngineVulkan.h"

#define GLFW_KEY_W 87
#define GLFW_KEY_S 83
#define GLFW_KEY_A 65
#define GLFW_KEY_D 68
#define GLFW_KEY_LEFT_BRACKET 91
#define GLFW_KEY_RIGHT_BRACKET 93
#define GLFW_KEY_MINUS 45
#define GLFW_KEY_EQUAL 61
#define GLFW_KEY_Q 81
#define GLFW_KEY_LEFT_CONTROL 341
#define GLFW_KEY_LEFT_ALT 342
#define GLFW_KEY_LEFT_SHIFT 340
#define GLFW_KEY_SPACE 32

class EmptySceneProvider : public SceneProvider
{
    virtual void drawDrawableObjects(VEngine::Renderer::RenderStageInterface * stage, VEngine::Renderer::DescriptorSetInterface * set, double scale) override
    {
    }
};

int main(int argc, char** argv)
{
    int width = 1920;
    int height = 1080;
    auto toolkit = new VulkanToolkit(width, height, false, "Test");
    toolkit->getMedia()->scanDirectory("../../media");
    toolkit->getMedia()->scanDirectory("../../shaders");

    auto eventBus = new EventBus();

    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer(eventBus);
    galaxy->loadFromDatabase(galaxydb);

    auto cosmosRenderer = new CosmosRenderer(toolkit, eventBus, galaxy, width, height);
    cosmosRenderer->setExposure(0.001);

    Camera* camera = new Camera();
    camera->createProjectionPerspective(90.0, 16.0 / 9.0, 0.001f, 60000.0f);
    EmptySceneProvider* scene = new EmptySceneProvider();

    auto keyboard = toolkit->getKeyboard();
    auto mouse = toolkit->getMouse();

    keyboard->setOnKeyPressHandler([&](int key) {

    });

    keyboard->setOnKeyReleaseHandler([&](int key) {

    });

    keyboard->setOnKeyRepeatHandler([&](int key) {
    });

    keyboard->setOnCharHandler([&](unsigned int key) {
    });


    mouse->setOnMouseDownHandler([&](int key) {
    });

    mouse->setOnMouseUpHandler([&](int key) {
    });

    mouse->setOnMouseScrollHandler([&](double x, double y) {
    });

    float pitch = 0.0;
    float yaw = 0.0;
    float fov = 90.0;
    int lastcx = 0, lastcy = 0;

    int targetStar = 210;
    int targetPlanet = 5;
    int targetMoon = 0;

    auto targetBody = galaxy->getByPath(targetStar, targetPlanet, targetMoon);
    auto center = targetBody->getPosition(0);
    auto dist = targetBody->radius;

    camera->setPosition(center + glm::dvec3(dist * 1.03, 0.0, 0.0));

    int frames = 0;
    double lastTimeX = 0.0;
    double lastTimeFloored = 0.0;
    bool pauseSearchingGalaxy = false;
    while (!toolkit->shouldCloseWindow()) {
        frames++;
        double time = toolkit->getExecutionTime();
        double elapsed = time - lastTimeX;
        lastTimeX = time;
        double floored = floor(time);
        if (floored != lastTimeFloored) {
            printf("FPS %d, Memory in MB: %d\n", frames, (toolkit->getTotalAllocatedMemory() / 1024 / 1024));
            frames = 0;
        }
        lastTimeFloored = floored;
        camera->updateFrustumCone();

        double speed = 1.1f * elapsed * 100.0;
        if (keyboard->isKeyDown(GLFW_KEY_LEFT_CONTROL)) {
            speed *= 0.1f;
        }
        if (keyboard->isKeyDown(GLFW_KEY_LEFT_ALT)) {
            speed *= 10;
        }
        if (keyboard->isKeyDown(GLFW_KEY_LEFT_SHIFT)) {
            speed *= 730.1f;
        }
        if (keyboard->isKeyDown(GLFW_KEY_SPACE)) {
            speed *= 9730.1f;
            pauseSearchingGalaxy = true;
        }
        else {
            pauseSearchingGalaxy = false;
        }

        if (!pauseSearchingGalaxy) galaxy->update(camera->getPosition(), time);
        cosmosRenderer->updateCameraBuffer(camera, time);
        cosmosRenderer->draw(scene, time);
        glm::dvec3 dw = glm::vec3(0);
        double w = 0.0;
        if (keyboard->isKeyDown(GLFW_KEY_W)) {
            dw += glm::vec3(0, 0, -1);
            w += 1.0;
        }
        if (keyboard->isKeyDown(GLFW_KEY_S)) {
            dw += glm::vec3(0, 0, 1);
            w += 1.0;
        }
        if (keyboard->isKeyDown(GLFW_KEY_A)) {
            dw += glm::vec3(-1, 0, 0);
            w += 1.0;
        }
        if (keyboard->isKeyDown(GLFW_KEY_D)) {
            dw += glm::vec3(1, 0, 0);
            w += 1.0;
        }
        if (keyboard->isKeyDown(GLFW_KEY_LEFT_BRACKET)) {
            fov += 51.0f * elapsed;
            fov = glm::clamp(fov, 0.1f, 179.0f);
            camera->createProjectionPerspective(fov, 16.0 / 9.0, 0.001f, 60000.0f);
        }
        if (keyboard->isKeyDown(GLFW_KEY_RIGHT_BRACKET)) {
            fov -= 51.0f * elapsed;
            fov = glm::clamp(fov, 0.1f, 179.0f);
            camera->createProjectionPerspective(fov, 16.0 / 9.0, 0.001f, 60000.0f);
        }
        if (keyboard->isKeyDown(GLFW_KEY_MINUS)) {
            cosmosRenderer->setExposure(glm::max(0.0, cosmosRenderer->getExposure() - 0.00001));
        }
        if (keyboard->isKeyDown(GLFW_KEY_EQUAL)) {
            cosmosRenderer->setExposure(cosmosRenderer->getExposure() + 0.0001);
        }
        if (keyboard->isKeyDown(GLFW_KEY_Q)) {
            auto body = galaxy->getClosestCelestialBody();
            auto dir = glm::normalize(body.getPosition(time) - camera->getPosition());
            camera->setPosition(body.getPosition(time) - dir * body.radius * 2.0);
        }
        glm::dvec3 a = dw / w;
        glm::dvec3 dir = w > 0.0 ? a : dw;
        glm::dvec3 newpos = camera->getPosition();
        dir = glm::mat3_cast(camera->getOrientation()) * dir;
        newpos += length(dir) > 0.0 ? (normalize(dir) * speed) : (glm::dvec3(0.0));
        glm::dvec2 cursor;
        auto tup = mouse->getCursorPosition();
        cursor.x = std::get<0>(tup);
        cursor.y = std::get<1>(tup);

        float dx = (float)(lastcx - cursor.x);
        float dy = (float)(lastcy - cursor.y);

        lastcx = cursor.x;
        lastcy = cursor.y;
        yaw += dy * 0.2f;
        pitch += dx * 0.2f;
        if (yaw < -90.0) yaw = -90;
        if (yaw > 90.0) yaw = 90;
        if (pitch < -360.0f) pitch += 360.0f;
        if (pitch > 360.0f) pitch -= 360.0f;
        glm::quat newrot = glm::angleAxis(glm::radians(pitch), glm::vec3(0, 1, 0)) * glm::angleAxis(glm::radians(yaw), glm::vec3(1, 0, 0));
        camera->setOrientation(newrot);
        camera->setPosition(newpos);

        toolkit->poolEvents();
    }
    return 0;
}

