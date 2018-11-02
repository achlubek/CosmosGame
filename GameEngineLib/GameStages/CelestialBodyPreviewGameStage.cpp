#include "stdafx.h"
#include "CelestialBodyPreviewGameStage.h"

CelestialBodyPreviewGameStage::CelestialBodyPreviewGameStage(GameContainer* container, int64_t itargetStar, int64_t itargetPlanet, int64_t itargetMoon)
    : AbsGameStage(container), targetStar(itargetStar), targetPlanet(itargetPlanet), targetMoon(itargetMoon)
{
    INIReader* configreader = new INIReader(container->getVulkanToolkit()->getMedia(), "settings.ini");

    getViewCamera()->setStrategy(new ManualCameraStrategy());
    targetBody = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getByPath(targetStar, targetPlanet, targetMoon);
    center = targetBody->getPosition(0.0);
    dist = targetBody->radius;

    //    printf("Velocity %f \n", targetBody.calculateOrbitVelocity(0.0001));
    mindist = dist + 0.001;
    dist *= 2.0;

    getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->update(center, 0.0);

    // TODO : INTERPOLATOR
    getCosmosGameContainer()->getControls()->getRawKeyboard()->onKeyPress.add([&](int key) {
        if (key == GLFW_KEY_PAUSE) getCosmosGameContainer()->getCosmosRenderer()->recompileShaders(true);

        if (key == GLFW_KEY_9 || key == GLFW_KEY_0) {
            // well todo
        }
    });


    getGameContainer()->getControls()->onKeyDown.add([&](std::string key) {
        if (key == "time_scale_x1") {
            setTimeScale(1.0);
            getGameContainer()->getLogger()->log(LogSeverity::Normal, "Setting time scale to 1.0");
        }
        if (key == "time_scale_x10") {
            setTimeScale(10000.0);
            getGameContainer()->getLogger()->log(LogSeverity::Normal, "Setting time scale to 10000.0");
        }
        if (key == "time_scale_x100") {
            setTimeScale(1000000.0);
            getGameContainer()->getLogger()->log(LogSeverity::Normal, "Setting time scale to 1000000.0");
        }
        if (key == "time_scale_x1000") {
            setTimeScale(1000000000.0);
            getGameContainer()->getLogger()->log(LogSeverity::Normal, "Setting time scale to 1000000000.0");
        }
    });

    getViewCamera()->setFov(fov);
}


CelestialBodyPreviewGameStage::~CelestialBodyPreviewGameStage()
{
}

GameContainer * CelestialBodyPreviewGameStage::getCosmosGameContainer()
{
    return static_cast<GameContainer*>(getGameContainer());
}

void CelestialBodyPreviewGameStage::onSwitchTo()
{
}

void CelestialBodyPreviewGameStage::onSwitchFrom()
{
}

void CelestialBodyPreviewGameStage::onDraw()
{
}

void CelestialBodyPreviewGameStage::onUpdate(double elapsed)
{

    auto keyboard = getCosmosGameContainer()->getControls()->getRawKeyboard();
    center = targetBody->getPosition(getTimeProvider()->getTime());

    float orbitSpeed = elapsed * 2.0131;
    float panSpeed = elapsed * fov * 0.01f;
    float zoomSpeed = elapsed;
    /*
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
    }*/


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
        dist += glm::sqrt(dist) * 1.1;
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
        e -= e * 0.5;
        e = glm::clamp(e, 0.0000001, 10.0);
        getCosmosGameContainer()->getCosmosRenderer()->setExposure(e);
    }
    if (keyboard->getKeyStatus(GLFW_KEY_EQUAL) == GLFW_PRESS) {
        double e = getCosmosGameContainer()->getCosmosRenderer()->getExposure();
        e += e * 0.5;
        e = glm::clamp(e, 0.0000001, 10.0);
        getCosmosGameContainer()->getCosmosRenderer()->setExposure(e);
    }


    auto rotxmat = glm::angleAxis(xrot, glm::dvec3(1.0, 0.0, 0.0));
    auto rotymat = glm::angleAxis(yrot, glm::dvec3(0.0, 1.0, 0.0));

    auto Rrotxmat = glm::angleAxis(-Rxrot, glm::dvec3(1.0, 0.0, 0.0));
    auto Rrotymat = glm::angleAxis(-Ryrot, glm::dvec3(0.0, 1.0, 0.0));
    auto Rrotzmat = glm::angleAxis(-Rzrot, glm::dvec3(0.0, 0.0, 1.0));

    auto bodyRotation = glm::dquat(1.0, 0.0, 0.0, 0.0);
    if (targetPlanet > 0) {
        double now = getTimeProvider()->getTime();
        auto rottt = static_cast<CelestialBody*>(targetBody)
            ->getRotationMatrix(now);
        bodyRotation = glm::inverse(glm::dquat(glm::quat_cast(rottt)));
    }

    glm::dvec3 dir = bodyRotation * (rotymat * (rotxmat * glm::dvec3(0.0, 0.0, -1.0)));

    getViewCamera()->getCamera()->setPosition(center - dir * dist);
    getViewCamera()->getCamera()->setOrientation(bodyRotation * rotymat * rotxmat * Rrotzmat * Rrotymat * Rrotxmat);

}

void CelestialBodyPreviewGameStage::onUpdateObject(GameObject * object, double elapsed)
{
}

void CelestialBodyPreviewGameStage::onKeyDown(std::string key)
{
}

void CelestialBodyPreviewGameStage::onKeyUp(std::string key)
{
}
