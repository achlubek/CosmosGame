#include "stdafx.h"
#include "UtilitiesLib.h"
#include "CosmosRendererLib.h"
#include "SQLiteDatabase.h"

class EmptySceneProvider : public SceneProvider
{
    virtual void drawDrawableObjects(VEngine::Renderer::VulkanRenderStage * stage, VEngine::Renderer::VulkanDescriptorSet * set, double scale) override
    {
    }
};

int main(int argc, char** argv)
{
    int width = 1920;
    int height = 1080;
    auto vulkanToolkit = new VulkanToolkit(width, height, false, "Test");
    vulkanToolkit->getMedia()->scanDirectory("../../media");
    vulkanToolkit->getMedia()->scanDirectory("../../shaders");

    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(galaxydb);

    auto cosmosRenderer = new CosmosRenderer(vulkanToolkit, galaxy, width, height);
    cosmosRenderer->setExposure(0.001);

    Camera* camera = new Camera();
    camera->createProjectionPerspective(90.0, 16.0 / 9.0, 0.001f, 60000.0f);
    EmptySceneProvider* scene = new EmptySceneProvider();

    auto keyboard = vulkanToolkit->getKeyboard();
    auto mouse = vulkanToolkit->getMouse();

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
    while (!vulkanToolkit->shouldCloseWindow()) {
        frames++;
        double time = glfwGetTime();
        double elapsed = time - lastTimeX;
        lastTimeX = time;
        double floored = floor(time);
        if (floored != lastTimeFloored) {
            printf("FPS %d, Memory in MB: %d\n", frames, (vulkanToolkit->getTotalAllocatedMemory() / 1024 / 1024));
            frames = 0;
        }
        lastTimeFloored = floored;
        camera->updateFrustumCone();

        double speed = 1.1f * elapsed * 100.0;
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            speed *= 0.1f;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            speed *= 10;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speed *= 730.1f;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_SPACE) == GLFW_PRESS) {
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
        if (keyboard->getKeyStatus(GLFW_KEY_W) == GLFW_PRESS) {
            dw += glm::vec3(0, 0, -1);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_S) == GLFW_PRESS) {
            dw += glm::vec3(0, 0, 1);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_A) == GLFW_PRESS) {
            dw += glm::vec3(-1, 0, 0);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_D) == GLFW_PRESS) {
            dw += glm::vec3(1, 0, 0);
            w += 1.0;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
            fov += 51.0f * elapsed;
            fov = glm::clamp(fov, 0.1f, 179.0f);
            camera->createProjectionPerspective(fov, 16.0 / 9.0, 0.001f, 60000.0f);
        }
        if (keyboard->getKeyStatus(GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
            fov -= 51.0f * elapsed;
            fov = glm::clamp(fov, 0.1f, 179.0f);
            camera->createProjectionPerspective(fov, 16.0 / 9.0, 0.001f, 60000.0f);
        }
        if (keyboard->getKeyStatus(GLFW_KEY_MINUS) == GLFW_PRESS) {
            cosmosRenderer->setExposure(max(0.0, cosmosRenderer->getExposure() - 0.00001));
        }
        if (keyboard->getKeyStatus(GLFW_KEY_EQUAL) == GLFW_PRESS) {
            cosmosRenderer->setExposure(cosmosRenderer->getExposure() + 0.0001);
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

        vulkanToolkit->poolEvents();
    }
    return 0;
}

