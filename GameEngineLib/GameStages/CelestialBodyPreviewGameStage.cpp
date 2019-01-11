#include "stdafx.h"
#include "CelestialBodyPreviewGameStage.h"

CelestialBodyPreviewGameStage::CelestialBodyPreviewGameStage(GameContainer* container, int64_t itargetStar, int64_t itargetPlanet, int64_t itargetMoon)
    : AbsGameStage(container), targetStar(itargetStar), targetPlanet(itargetPlanet), targetMoon(itargetMoon)
{
    INIReader* configreader = new INIReader(container->getToolkit()->getMedia(), "settings.ini");

    getViewCamera()->setStrategy(new ManualCameraStrategy());
    targetBody = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getByPath(targetStar, targetPlanet, targetMoon);
    center = targetBody->getPosition(0.0);
    dist = targetBody->radius;

    //    printf("Velocity %f \n", targetBody.calculateOrbitVelocity(0.0001));
    mindist = dist + 0.001;
    dist *= 2.0;

    getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->update(center, 0.0);

    // TODO : INTERPOLATOR
    /*getCosmosGameContainer()->getControls()->getRawKeyboard()->onKeyPress.add([&](int key) {
        if (key == KEY_PAUSE) getCosmosGameContainer()->getCosmosRenderer()->recompileShaders(true);

        if (key == KEY_9 || key == KEY_0) {
            // well todo
        }
    });
    */
    /*
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
    });*/

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

    auto keyboard = getCosmosGameContainer()->getControls();
    center = targetBody->getPosition(getTimeProvider()->getTime());

    float orbitSpeed = elapsed * 2.0131;
    float panSpeed = elapsed * fov * 0.01f;
    float zoomSpeed = elapsed;
    
    /*if (keyboard->isKeyDown("time_scale_x1")) {
        timeScale = 0.001;
    }
    if (keyboard->isKeyDown("time_scale_x10")) {
        timeScale = 0.01;
    }
    if (keyboard->isKeyDown("time_scale_x100")) {
        timeScale = 0.5;
    }
    if (keyboard->isKeyDown("time_scale_x1000")) {
        timeScale = 4.0;
    }*/

    
    if (keyboard->isKeyDown("celestial_preview_pan_up")) {
        xrot -= orbitSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_pan_down")) {
        xrot += orbitSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_pan_left")) {
        yrot -= orbitSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_pan_right")) {
        yrot += orbitSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_rotate_up")) {
        Rxrot -= panSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_rotate_down")) {
        Rxrot += panSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_rotate_left")) {
        Ryrot -= panSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_rotate_right")) {
        Ryrot += panSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_rotate_roll_left")) {
        Rzrot -= panSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_rotate_roll_right")) {
        Rzrot += panSpeed;
    }
    if (keyboard->isKeyDown("celestial_preview_closer")) {
        dist = dist * 0.95 + mindist * 0.05;
    }
    if (keyboard->isKeyDown("celestial_preview_further")) {
        dist += glm::sqrt(dist) * 1.1;
    }
    if (keyboard->isKeyDown("celestial_preview_fov_add")) {
        fov += 51.0f * elapsed;
        fov = glm::clamp(fov, 0.1f, 179.0f);
        getViewCamera()->setFov(fov);
    }
    if (keyboard->isKeyDown("celestial_preview_fov_sub")) {
        fov -= 51.0f * elapsed;
        fov = glm::clamp(fov, 0.1f, 179.0f);
        getViewCamera()->setFov(fov);
    }
    if (keyboard->isKeyDown("celestial_preview_exposure_add")) {
        double e = getCosmosGameContainer()->getCosmosRenderer()->getExposure();
        e += e * 0.5;
        e = glm::clamp(e, 0.0000001, 10.0);
        getCosmosGameContainer()->getCosmosRenderer()->setExposure(e);
    }
    if (keyboard->isKeyDown("celestial_preview_exposure_sub")) {
        double e = getCosmosGameContainer()->getCosmosRenderer()->getExposure();
        e -= e * 0.5;
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
