#include "stdafx.h"
#include "FreeFlightGameStage.h"

#include "Transformation3DComponent.h"
#include "ThrustGeneratorComponent.h"
#include "GeneratedStarInfo.h"
#include "CelestialBody.h"
#include "GameContainer.h"
#include "ShipFactory.h"
#include "PlayerFactory.h"
#include "TimeProvider.h"
#include "CosmosRenderer.h"
#include "GalaxyContainer.h"
#include "GameControls.h"
#include "CameraController.h"
#include "AbsGameContainer.h"
#include "ModelsRenderer.h"
#include "CameraChaseStrategy.h"
#include "CameraFirstPersonStrategy.h"
#include "ParticleSystem.h"
#include "ParticlesRenderer.h"

FreeFlightGameStage::FreeFlightGameStage(AbsGameContainer* container)
    : AbsGameStage(container)
{
    auto ui = getUIRenderer();
    fpsText = new UIText(ui, 0.01, 0.0, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("chintzy.ttf"), 13, "Hmm");
    ui->addDrawable(fpsText);

    gravityFluxText = new UIText(ui, 0.01, 0.028, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("Sansation_Regular.ttf"), 23, "Hmm");
    ui->addDrawable(gravityFluxText);

    starNameText = new UIText(ui, 0.01, 0.028 * 2.0, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("Sansation_Regular.ttf"), 23, "Hmm");
    ui->addDrawable(starNameText);

    planetNameText = new UIText(ui, 0.01, 0.028 * 3.0, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("Sansation_Regular.ttf"), 23, "Hmm");
    ui->addDrawable(planetNameText);

    moonNameText = new UIText(ui, 0.01, 0.028 * 4.0, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("Sansation_Regular.ttf"), 23, "Hmm");
    ui->addDrawable(moonNameText);

    altitudeText = new UIText(ui, 0.01, 0.028 * 5.0, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("Sansation_Regular.ttf"), 23, "Hmm");
    ui->addDrawable(altitudeText);

    velocityText = new UIText(ui, 0.01, 0.028 * 6.0, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("Sansation_Regular.ttf"), 23, "Hmm");
    ui->addDrawable(velocityText);

    // a test
    auto testship = getCosmosGameContainer()->getShipFactory()->build("Orbiter1.ship_blueprint.json");
    //auto testspawnpos = cosmosRenderer->galaxy->getAllStars()[666].getPosition(0);
    //auto testspawnradius = cosmosRenderer->galaxy->getAllStars()[666].radius;
    //cosmosRenderer->galaxy->update(testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getPosition());
    int targetStar = 333;
    int targetPlanet = 3;
    int targetMoon = 0;
    auto galaxy = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy();
    getCosmosGameContainer()->getCosmosRenderer()->setExposure(0.00002);

    auto targetBody = galaxy->getByPath(targetStar, targetPlanet, targetMoon);
    auto center = targetBody->getPosition(0);
    auto dist = targetBody->radius;
    auto velocity = targetBody->getLinearVelocity(getTimeProvider()->getTime());

    galaxy->onClosestStarChange.add([&](GeneratedStarInfo star) {
        getCosmosGameContainer()->getCosmosRenderer()->invokeOnDrawingThread([=]() {
            starNameText->updateText("Star: " + std::to_string(star.starId));
        });
    });

    galaxy->onClosestPlanetChange.add([&](CelestialBody body) {
        getCosmosGameContainer()->getCosmosRenderer()->invokeOnDrawingThread([=]() {
         //   planetNameText->updateText("Planet: " + std::to_string(body.));
        });
    });

    getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->onClosestMoonChange.add([&](CelestialBody body) {
        getCosmosGameContainer()->getCosmosRenderer()->invokeOnDrawingThread([=]() {
         //   moonNameText->updateText("Moon: " + name);
        });
    });

    getCosmosGameContainer()->getControls()->onKeyDown.add([&](std::string key) {
        if (key == "recompile_shaders") getCosmosGameContainer()->getCosmosRenderer()->recompileShaders(true);
    });

    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(center + glm::dvec3(0.0, dist * 1.043, 0.0));
    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(velocity + 1000.0 * targetBody->calculateOrbitVelocity(dist * 0.03) * glm::dvec3(1.0, 0.0, 0.0));
    //testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(glm::dvec3(0.0));

    addObject(testship);

    auto player = getCosmosGameContainer()->getPlayerFactory()->build();

    player->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(center + glm::dvec3(0.0, dist * 1.043, 0.0));
    player->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(velocity + 1000.0 * targetBody->calculateOrbitVelocity(dist * 0.03) * glm::dvec3(1.0, 0.0, 0.0));
    //testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(glm::dvec3(0.0));

    addObject(player);

    getViewCamera()->setTarget(player);
    getViewCamera()->setStrategy(new CameraFirstPersonStrategy());
    getViewCamera()->setFov(66.0);

    getGameContainer()->getControls()->onKeyDown.add([&](std::string key) {
        if (key == "time_scale_x1") {
            setTimeScale(1.0);
        }
        if (key == "time_scale_x10") {
            setTimeScale(10.0);
        }
        if (key == "time_scale_x100") {
            setTimeScale(100.0);
        }
        if (key == "time_scale_x1000") {
            setTimeScale(1000.0);
        }
    });
}


FreeFlightGameStage::~FreeFlightGameStage()
{
}

GameContainer * FreeFlightGameStage::getCosmosGameContainer()
{
    return static_cast<GameContainer*>(getGameContainer());
}

void FreeFlightGameStage::onSwitchTo()
{

}
void FreeFlightGameStage::onSwitchFrom()
{

}
void FreeFlightGameStage::onDraw()
{
}

void FreeFlightGameStage::onUpdate(double elapsed)
{/*
    unsigned int componentId = 
        getGameContainer()->getModelsRenderer()
        ->pickComponentId(
            glm::vec2(
                getGameContainer()->getControls()->getCursorPosition())
                / glm::vec2(getGameContainer()->getVulkanToolkit()->windowWidth, getGameContainer()->getVulkanToolkit()->windowHeight
                )
        );
    printf("componentid %d\n", componentId);*/
}

void FreeFlightGameStage::onUpdateObject(GameObject * object, double elapsed)
{
    auto cosmosRenderer = getCosmosGameContainer()->getCosmosRenderer();
    auto physicsComponent = object->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);
    if (nullptr != physicsComponent) {
        physicsComponent->setTimeScale(0.001);
        auto g = cosmosRenderer->getGalaxy()->getGravity(physicsComponent->getPosition(), getTimeProvider()->getTime());
        physicsComponent->applyGravity(g);
        gravityFluxText->updateText(std::to_string(glm::length(g)));
        auto relativeVel = cosmosRenderer->getGalaxy()->getClosestCelestialBody().getRelativeLinearVelocity(physicsComponent->getLinearVelocity() * 0.001, getTimeProvider()->getTime());
        velocityText->updateText("Relative velocity M/S: " + std::to_string(1000.0 * glm::length(relativeVel)));
        auto body = cosmosRenderer->getGalaxy()->getClosestCelestialBody();
        auto position = body.getPosition(getTimeProvider()->getTime());
        auto waterLevel = body.radius + body.fluidMaxLevel;
        auto atmoLevel = body.radius + body.atmosphereRadius;
        auto dirToShip = glm::normalize(physicsComponent->getPosition() - position);
        auto dist = glm::distance(position, physicsComponent->getPosition()) - waterLevel;
        auto distatm = glm::distance(position, physicsComponent->getPosition()) - atmoLevel;
        altitudeText->updateText("Altitude KM: " + std::to_string(dist));
        auto airVelocity = body.getSurfaceVelocityAtPoint(physicsComponent->getPosition(), getTimeProvider()->getTime());
        if (dist < 0.0) {
            physicsComponent->setLinearVelocity(glm::mix(physicsComponent->getLinearVelocity(), airVelocity * 1000.0, elapsed * 2.0 * min(1.0, -dist * 1000.0)));
            physicsComponent->setLinearVelocity(physicsComponent->getLinearVelocity() + dirToShip * elapsed * 10.0 * min(1.0, -dist * 1000.0));
            //physicsComponent->applyAbsoluteImpulse(glm::dvec3(0.0), -physicsComponent->getLinearVelocity() * min(1.0, -dist * 1000.0) * 15000000.0 * elapsed);
            // physicsComponent->applyAbsoluteImpulse(glm::dvec3(0.0), dirToShip * 5000000.0 * min(1.0, -dist * 1000.0) * elapsed);
        }
        if (distatm < 0.0) {
            auto relativeVel = physicsComponent->getLinearVelocity() - airVelocity * 1000.0;
            physicsComponent->applyAbsoluteImpulse(glm::dvec3(0.0), relativeVel * elapsed * -1000.0);
        }
    }
}
