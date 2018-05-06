#include "stdafx.h"
#include "AbsGameContainer.h"
#include "GameControls.h"
#include "TimeProvider.h"
#include "CameraController.h"
#include "ManualCameraStrategy.h"
#include "PointerDrivenCameraStrategy.h"
#include "CinematicScenarioReader.h"
#include "AbsGameStage.h"
#include "GameContainer.h"
#include "FreeFlightGameStage.h"
#include "MainMenuGameStage.h"
#include "CinematicAnimationGameStage.h"

int64_t targetStar = 0;
int64_t targetPlanet = 0;
int64_t targetMoon = 0;

class EditorGameStage : public AbsGameStage
{
public:
    EditorGameStage(AbsGameContainer* container)
        : AbsGameStage(container)
    {
        INIReader* configreader = new INIReader("settings.ini");

        getViewCamera()->setStrategy(new ManualCameraStrategy());
        targetBody = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getByPath(targetStar, targetPlanet, targetMoon);
        center = targetBody->getPosition(0.0);
        dist = targetBody->radius;

        //    printf("Velocity %f \n", targetBody.calculateOrbitVelocity(0.0001));
        mindist = dist + 0.001;
        dist *= 2.0;

        getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->update(center);

        // TODO : INTERPOLATOR
        getCosmosGameContainer()->getControls()->getRawKeyboard()->onKeyPress.add([&](int key) {
            if (key == GLFW_KEY_PAUSE) getCosmosGameContainer()->getCosmosRenderer()->recompileShaders(true);
            if (key == GLFW_KEY_O) {
                auto pointerdrivencamera = new PointerDrivenCameraStrategy();
                getViewCamera()->setStrategy(pointerdrivencamera);
                auto scenario = CinematicScenarioReader(getCosmosGameContainer()->getInterpolator(), getCosmosGameContainer()->getCosmosRenderer(), pointerdrivencamera, getTimeProvider());
                disableThings = true;
                scenario.load("cinematic_scenario.txt", getTimeProvider()->getTime());
                scenario.execute();
            }
            if (key == GLFW_KEY_9 || key == GLFW_KEY_0) {
                getCosmosGameContainer()->getCosmosRenderer()->invokeOnDrawingThread([&, key]() {
                    star = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getAllStars()[targetStar - 1];
                    auto planet = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getClosestPlanet();
                    auto moon = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getClosestMoon();
                    /*traating limitors as 3d vector

                    xyz: {starsCount, closestStarPlanetsCount, closestPlanetMoonsCount}
                    allowed : ( {0,0,0} -> xyz >


                    */

                    int32_t x = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getAllStars().size();
                    int32_t y = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getClosestStarPlanets().size();
                    int32_t z = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getClosestPlanetMoons().size();

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
                    targetBody = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getByPath(targetStar, targetPlanet, targetMoon);
                    center = targetBody->getPosition(0.0);
                    dist = targetBody->radius;

                    //    printf("Velocity %f \n", targetBody.calculateOrbitVelocity(0.0001));
                    mindist = dist + 0.001;
                    dist *= 2.0;
                    printf("Limits\t%d,\t%d,\t%d\n", x, y, z);
                    printf("Value\t%d,\t%d,\t%d\n", cx, cy, cz);
                    mindist = dist + 0.001;
                    dist *= 2.0;
                });
            }
        });

        getViewCamera()->setFov(fov);
    }
    AbsCelestialObject* targetBody;
    CelestialBody celestialTarget;
    GeneratedStarInfo star;
    glm::dvec3 center = glm::dvec3(0.0);
    double dist = 0.0;
    float fov = 90.0f;
    float mindist = 100.0;

    double xrot = 0.0;
    double yrot = 0.0;
    double zrot = 0.0;

    double Rxrot = 0.0;
    double Ryrot = 0.0;
    double Rzrot = 0.0;

    double timeScale = 0.1;

    bool disableThings = false; 

    GameContainer * getCosmosGameContainer()
    {
        return static_cast<GameContainer*>(getGameContainer());
    }
    virtual void onSwitchTo() override {}
    virtual void onSwitchFrom() override {}


    virtual void onDraw() override
    {
    }
    virtual void onUpdate(double elapsed) override
    {
        if (disableThings) {
            getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->update(getViewCamera()->getPosition());
            return;

        }
        auto keyboard = getCosmosGameContainer()->getControls()->getRawKeyboard();
        center = targetBody->getPosition(getTimeProvider()->getTime());

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
            getViewCamera()->setFov(fov);
        }
        if (keyboard->getKeyStatus(GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
            fov -= 51.0f * elapsed;
            fov = glm::clamp(fov, 0.1f, 179.0f);
            getViewCamera()->setFov(fov);
        }
        if (keyboard->getKeyStatus(GLFW_KEY_MINUS) == GLFW_PRESS) {
            double e = getCosmosGameContainer()->getCosmosRenderer()->getExposure();
            e -= 0.00001;
            e = glm::clamp(e, 0.0000001, 10.0);
            getCosmosGameContainer()->getCosmosRenderer()->setExposure(e);
        }
        if (keyboard->getKeyStatus(GLFW_KEY_EQUAL) == GLFW_PRESS) {
            double e = getCosmosGameContainer()->getCosmosRenderer()->getExposure();
            e += 0.00001;
            e = glm::clamp(e, 0.0000001, 10.0);
            getCosmosGameContainer()->getCosmosRenderer()->setExposure(e);
        }


        auto rotxmat = glm::angleAxis(xrot, glm::dvec3(1.0, 0.0, 0.0));
        auto rotymat = glm::angleAxis(yrot, glm::dvec3(0.0, 1.0, 0.0));

        auto Rrotxmat = glm::angleAxis(-Rxrot, glm::dvec3(1.0, 0.0, 0.0));
        auto Rrotymat = glm::angleAxis(-Ryrot, glm::dvec3(0.0, 1.0, 0.0));
        auto Rrotzmat = glm::angleAxis(-Rzrot, glm::dvec3(0.0, 0.0, 1.0));

        auto bodyRotation = targetPlanet == 0 ? glm::dquat(1.0, 0.0, 0.0, 0.0) : glm::inverse(glm::dquat(glm::quat_cast(static_cast<CelestialBody*>(targetBody)->getRotationMatrix(getTimeProvider()->getTime()))));

        glm::dvec3 dir = bodyRotation * (rotymat * (rotxmat * glm::dvec3(0.0, 0.0, -1.0)));

        getViewCamera()->setPosition(center - dir * dist);
        getViewCamera()->setOrientation(bodyRotation * rotymat * rotxmat * Rrotzmat * Rrotymat * Rrotxmat);

    }
    virtual void onUpdateObject(GameObject * object, double elapsed) override
    {
    }
};

int main(int argc, char** argv)
{

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


    GameContainer* game = new GameContainer();

    EditorGameStage* stage = new EditorGameStage(game);
    FreeFlightGameStage* stageflight = new FreeFlightGameStage(game);
    MainMenuGameStage* menustage = new MainMenuGameStage(game);
    CinematicAnimationGameStage* intro = new CinematicAnimationGameStage(game, "cinematic_scenario.txt", "mainmenu");

    game->registerStage("freeflight", stageflight);
    game->registerStage("preview", stage);
    game->registerStage("mainmenu", menustage);
    game->registerStage("intro-animation", intro);

    game->setCurrentStage("intro-animation");

    game->startGameLoops();


    return 0;
}

