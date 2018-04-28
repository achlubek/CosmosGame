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
    int64_t targetStar = 0;
    int64_t targetPlanet = 0;
    int64_t targetMoon = 0;

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



    INIReader* configreader = new INIReader("settings.ini");
    auto vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"), false, "Galaxy Game");


    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    auto database = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(database);
    UIRenderer* ui = new UIRenderer(vulkanToolkit, mouse, configreader->geti("window_width"), configreader->geti("window_height"));

    auto timeProvider = new TimeProvider();
    auto cosmosRenderer = new CosmosRenderer(vulkanToolkit, timeProvider, new EmptySceneProvider(), galaxy, ui->outputImage, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    glm::dvec3 center = glm::dvec3(0.0);
    double dist = 0.0;

    GeneratedStarInfo star = galaxy->getAllStars()[targetStar - 1];
    center = star.getPosition(0);
    dist = star.radius;
    AbsCelestialObject* targetBody = &star;// = AbsCelestialObject();
    CelestialBody celestialTarget;
    if (targetPlanet > 0) {
        galaxy->update(star.getPosition(0));
        auto planet = galaxy->getClosestStarPlanets()[targetPlanet - 1];
        center = planet.getPosition(0);
        dist = planet.radius + planet.fluidMaxLevel;
        celestialTarget = planet;
        targetBody = &celestialTarget;
        if (targetMoon > 0) {
            galaxy->update(planet.getPosition(0));
            auto moon = galaxy->getClosestPlanetMoons()[targetMoon - 1];
            center = moon.getPosition(0);
            dist = moon.radius + moon.fluidMaxLevel;
            celestialTarget = moon;
            targetBody = &celestialTarget;
        }
    }

    //    printf("Velocity %f \n", targetBody.calculateOrbitVelocity(0.0001));
    float mindist = dist + 0.001;
    dist *= 2.0;

    galaxy->update(center);

    // TODO : INTERPOLATOR
    keyboard->onKeyPress.add([&](int key) {
        if (key == GLFW_KEY_PAUSE) cosmosRenderer->recompileShaders(true);
        if (key == GLFW_KEY_9 || key == GLFW_KEY_0) {
            cosmosRenderer->updatingSafetyQueue.enqueue([&, key]() {
                star = galaxy->getAllStars()[targetStar - 1];
                auto planet = galaxy->getClosestPlanet();
                auto moon = galaxy->getClosestMoon();
                /*traating limitors as 3d vector

                xyz: {starsCount, closestStarPlanetsCount, closestPlanetMoonsCount}
                allowed : ( {0,0,0} -> xyz >


                */

                int32_t x = galaxy->getAllStars().size();
                int32_t y = galaxy->getClosestStarPlanets().size();
                int32_t z = galaxy->getClosestPlanetMoons().size();

                int32_t cx = targetStar;
                int32_t cy = targetPlanet;
                int32_t cz = targetMoon;


                if (key == GLFW_KEY_0) {
                    cz--;
                }
                if (key == GLFW_KEY_9) {
                    cz++;
                }

                if (cz >= z) {
                    cy++;
                    cz = 0;
                }

                if (cz < 0) {
                    cy--;
                    cz = 100;
                }

                if (cy >= y) {
                    cx++;
                    cy = 0;
                }

                if (cy < 0) {
                    cx--;
                    cy = 100;
                }



                targetStar = cx;
                targetPlanet = cy;
                targetMoon = cz;

                star = galaxy->getAllStars()[targetStar - 1];
                center = star.getPosition(0);
                dist = star.radius;
                targetBody = &star;
                galaxy->update(star.getPosition(0));
                if (targetPlanet > galaxy->getClosestStarPlanets().size()) {
                    targetPlanet = galaxy->getClosestStarPlanets().size();
                }
                if (targetPlanet > 0) {
                    planet = galaxy->getClosestStarPlanets()[targetPlanet - 1];
                    center = planet.getPosition(0);
                    dist = planet.radius - planet.fluidMaxLevel;
                    celestialTarget = planet;
                    targetBody = &celestialTarget;
                    galaxy->update(planet.getPosition(0));
                    if (targetMoon > galaxy->getClosestPlanetMoons().size()) {
                        targetMoon = galaxy->getClosestPlanetMoons().size();
                    }
                    if (targetMoon > 0) {
                        moon = galaxy->getClosestPlanetMoons()[targetMoon - 1];
                        center = moon.getPosition(0);
                        dist = moon.radius - moon.fluidMaxLevel;
                        celestialTarget = moon;
                        targetBody = &celestialTarget;
                    }
                }
                printf("Limits\t%d,\t%d,\t%d\n", x, y, z);
                printf("Value\t%d,\t%d,\t%d\n", cx, cy, cz);
                mindist = dist + 0.001;
                dist *= 2.0;
            });
        }
    });


    Camera * cam = new Camera();
    float ratio = (float)configreader->geti("window_width") / (float)configreader->geti("window_height");
    float fov = 90.0f;
    cam->createProjectionPerspective(fov, ratio, 0.001f, 200000.0f);

    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStarsBuffer();

    double xrot = 0.0;
    double yrot = 0.0;
    double zrot = 0.0;

    double Rxrot = 0.0;
    double Ryrot = 0.0;
    double Rzrot = 0.0;

    double timeScale = 0.1;

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
        timeProvider->update(elapsed * timeScale);
        lastRawTime = time;
        lastTime = nowtime;

        center = targetBody->getPosition(timeProvider->getTime());

        float orbitSpeed = elapsed * 0.000131 * (dist - mindist);
        float panSpeed = elapsed * fov * 0.01f;
        float zoomSpeed = elapsed;

        if (keyboard->getKeyStatus(GLFW_KEY_F1) == GLFW_PRESS) {
            timeScale = 0.001;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_F2) == GLFW_PRESS) {
            timeScale = 0.01;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_F3) == GLFW_PRESS) {
            timeScale = 0.5;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_F4) == GLFW_PRESS) {
            timeScale = 4.0;
        }


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
        if (keyboard->getKeyStatus(GLFW_KEY_Q) == GLFW_PRESS) {
            Rzrot -= panSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_E) == GLFW_PRESS) {
            Rzrot += panSpeed;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            dist = dist * 0.95 + mindist * 0.05;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            dist += glm::sqrt(dist) * 0.1;
        }
        if (keyboard->getKeyStatus(GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
            fov += 51.0f * elapsed;
            fov = glm::clamp(fov, 0.1f, 179.0f);
            cam->createProjectionPerspective(fov, ratio, 0.001f, 200000.0f);
        }
        if (keyboard->getKeyStatus(GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
            fov -= 51.0f * elapsed;
            fov = glm::clamp(fov, 0.1f, 179.0f);
            cam->createProjectionPerspective(fov, ratio, 0.001f, 200000.0f);
        }
        if (keyboard->getKeyStatus(GLFW_KEY_MINUS) == GLFW_PRESS) {
            cosmosRenderer->exposure -= 0.00001;
            cosmosRenderer->exposure = glm::clamp(cosmosRenderer->exposure, 0.0000001f, 10.0f);
        }
        if (keyboard->getKeyStatus(GLFW_KEY_EQUAL) == GLFW_PRESS) {
            cosmosRenderer->exposure += 0.00001;
            cosmosRenderer->exposure = glm::clamp(cosmosRenderer->exposure, 0.0000001f, 10.0f);
        }


        auto rotxmat = glm::angleAxis(xrot, glm::dvec3(1.0, 0.0, 0.0));
        auto rotymat = glm::angleAxis(yrot, glm::dvec3(0.0, 1.0, 0.0));

        auto Rrotxmat = glm::angleAxis(-Rxrot, glm::dvec3(1.0, 0.0, 0.0));
        auto Rrotymat = glm::angleAxis(-Ryrot, glm::dvec3(0.0, 1.0, 0.0));
        auto Rrotzmat = glm::angleAxis(-Rzrot, glm::dvec3(0.0, 0.0, 1.0));

        auto bodyRotation = targetPlanet == 0 ? glm::dquat(1.0, 0.0, 0.0, 0.0) : glm::inverse(glm::dquat(glm::quat_cast(static_cast<CelestialBody*>(targetBody)->getRotationMatrix(timeProvider->getTime()))));

        glm::dvec3 dir = bodyRotation * (rotymat * (rotxmat * glm::dvec3(0.0, 0.0, -1.0)));
        cam->transformation->setOrientation(bodyRotation * rotymat * rotxmat * Rrotzmat * Rrotymat * Rrotxmat);

        ui->draw();
        cosmosRenderer->updateCameraBuffer(cam, center - dir * dist);
        cosmosRenderer->draw();
        vulkanToolkit->poolEvents();
    }

    return 0;
}

