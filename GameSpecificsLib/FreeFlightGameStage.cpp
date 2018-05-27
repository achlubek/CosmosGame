#include "stdafx.h"
#include "FreeFlightGameStage.h"

#include "Transformation3DComponent.h"
#include "ThrustGeneratorComponent.h"
#include "GeneratedStarInfo.h"
#include "CelestialBody.h"
#include "GameContainer.h"
#include "ShipFactory.h"
#include "TimeProvider.h"
#include "CosmosRenderer.h"
#include "GalaxyContainer.h"
#include "GameControls.h"
#include "CameraController.h"
#include "AbsGameContainer.h"
#include "ModelsRenderer.h"
#include "CameraChaseStrategy.h"
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
    auto testship = getCosmosGameContainer()->getShipFactory()->build("Generix Explorer 1.ship_blueprint.ini");
    //auto testspawnpos = cosmosRenderer->galaxy->getAllStars()[666].getPosition(0);
    //auto testspawnradius = cosmosRenderer->galaxy->getAllStars()[666].radius;
    //cosmosRenderer->galaxy->update(testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getPosition());
    int targetStar = 333;
    int targetPlanet = 4;
    int targetMoon = 0;
    auto galaxy = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy();

    auto targetBody = galaxy->getByPath(targetStar, targetPlanet, targetMoon);
    auto center = targetBody->getPosition(0);
    auto dist = targetBody->radius;
    auto velocity = targetBody->getLinearVelocity(getTimeProvider()->getTime());

    galaxy->onClosestStarChange.add([&](GeneratedStarInfo star) {
        auto name = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getStarName(star.starId);
        getCosmosGameContainer()->getCosmosRenderer()->invokeOnDrawingThread([=]() {
            starNameText->updateText("Star: " + std::to_string(star.starId) + " " + name);
        });
    });

    galaxy->onClosestPlanetChange.add([&](CelestialBody body) {
        auto name = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getCelestialBodyName(body.bodyId);
        getCosmosGameContainer()->getCosmosRenderer()->invokeOnDrawingThread([=]() {
            planetNameText->updateText("Planet: " + name);
        });
    });

    getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->onClosestMoonChange.add([&](CelestialBody body) {
        auto name = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->getCelestialBodyName(body.bodyId);
        getCosmosGameContainer()->getCosmosRenderer()->invokeOnDrawingThread([=]() {
            moonNameText->updateText("Moon: " + name);
        });
    });

    getCosmosGameContainer()->getControls()->onKeyDown.add([&](std::string key) {
        if (key == "recompile_shaders") getCosmosGameContainer()->getCosmosRenderer()->recompileShaders(true);
    });

    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(center + glm::dvec3(0.0, dist * 1.043, 0.0));
    //testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(velocity + 1000.0 * targetBody->calculateOrbitVelocity(dist * 0.03) * glm::dvec3(1.0, 0.0, 0.0));
    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(glm::dvec3(0.0));

    addObject(testship);
    getViewCamera()->setTarget(testship);
    getViewCamera()->setStrategy(new CameraChaseStrategy(false));

    auto smokeTexture = container->getAssetLoader()->loadTextureFile("smoke.png");

    smokeParticleSystem = new ParticleSystem(container->getVulkanToolkit(), smokeTexture, container->getParticlesRenderer()->getParticleLayout(),
        1000, //maxParticlesCount
        0.0002, //startSize
        1.0, //startTransparency
        10.01, //startRotationSpeed
        0.0, //startVelocity
        0.0002, //endSize
        0.0, //endTransparency
        0.0, //endRotationSpeed
        0.0, //endVelocity
        1.0, //lifeTime
        1.0);
    container->getParticlesRenderer()->registerParticleSystem(smokeParticleSystem);
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
    auto baseVelocity = physicsComponent->getLinearVelocity();
    auto basePosition = physicsComponent->getPosition();
    smokeParticleSystem->setGenerationTimeout(0.002);
    auto thrustGenerators = object->getAllComponentsByType<ThrustGeneratorComponent>(ComponentTypes::ThrustGenerator);
    if (smokeParticleSystem->ifTimeoutAllowsGeneration()) {
        smokeParticleSystem->generate(basePosition, baseVelocity * 0.0);
    }
    for (int i = 0; i < thrustGenerators.size(); i++) {
        auto t = thrustGenerators[i];
        auto vector = t->getThrustVector();

    }
}
