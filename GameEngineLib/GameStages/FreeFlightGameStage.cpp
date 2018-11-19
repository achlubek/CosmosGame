#include "stdafx.h"
#include "FreeFlightGameStage.h"

FreeFlightGameStage::FreeFlightGameStage(GameContainer* container)
    : AbsGameStage(container)
{
    auto ui = getUIRenderer();
    fpsText = new UIText(ui, 0.01, 0.0, UIColor(1.0, 1.0, 1.0, 1.0), "chintzy.ttf", 13, "Hmm");
    ui->addDrawable(fpsText);

    gravityFluxText = new UIText(ui, 0.01, 0.028, UIColor(1.0, 1.0, 1.0, 1.0), "Sansation_Regular.ttf", 23, "Hmm");
    ui->addDrawable(gravityFluxText);

    starNameText = new UIText(ui, 0.01, 0.028 * 2.0, UIColor(1.0, 1.0, 1.0, 1.0), "Sansation_Regular.ttf", 23, "Hmm");
    ui->addDrawable(starNameText);

    planetNameText = new UIText(ui, 0.01, 0.028 * 3.0, UIColor(1.0, 1.0, 1.0, 1.0), "Sansation_Regular.ttf", 23, "Hmm");
    ui->addDrawable(planetNameText);

    moonNameText = new UIText(ui, 0.01, 0.028 * 4.0, UIColor(1.0, 1.0, 1.0, 1.0), "Sansation_Regular.ttf", 23, "Hmm");
    ui->addDrawable(moonNameText);

    altitudeText = new UIText(ui, 0.01, 0.028 * 5.0, UIColor(1.0, 1.0, 1.0, 1.0), "Sansation_Regular.ttf", 23, "Hmm");
    ui->addDrawable(altitudeText);

    velocityText = new UIText(ui, 0.01, 0.028 * 6.0, UIColor(1.0, 1.0, 1.0, 1.0), "Sansation_Regular.ttf", 23, "Hmm");
    ui->addDrawable(velocityText);

    getCosmosGameContainer()->getControls()->onKeyDown.add([&](std::string key) {
        if (key == "recompile_shaders") getCosmosGameContainer()->getCosmosRenderer()->recompileShaders(true);
    });
    
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

    playerMountState = new PlayerMountState();
}


FreeFlightGameStage::~FreeFlightGameStage()
{
    safedelete(playerMountState);
    getUIRenderer()->removeAllDrawables();
    safedelete(fpsText);
    safedelete(gravityFluxText);
    safedelete(starNameText);
    safedelete(planetNameText);
    safedelete(moonNameText);
    safedelete(altitudeText);
    safedelete(velocityText);
}

void FreeFlightGameStage::initializeNew()
{
    int targetStar = 221;
    int targetPlanet = 3;
    int targetMoon = 0;
    auto galaxy = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy();
    getCosmosGameContainer()->getCosmosRenderer()->setExposure(0.00006);

    auto targetBody = galaxy->getByPath(targetStar, targetPlanet, targetMoon);
    auto center = targetBody->getPosition(0);
    auto dist = targetBody->radius;
    auto velocity = targetBody->getLinearVelocity(getTimeProvider()->getTime());

    auto testship = getCosmosGameContainer()->getShipFactory()->build("Orbiter1.ship_blueprint.json");
    testship->addTag(GameObjectTags::Ship);
    testship->addTag(GameObjectTags::ControllableVehicle);

    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(center - glm::dvec3(dist * 1.03, 0.0, 0.0));
    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(velocity + 1000.0 * targetBody->calculateOrbitVelocity(dist * 0.03) * glm::dvec3(0.0, 1.0, 0.0));
    //testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(glm::dvec3(0.0));

    addObject(testship);

    auto player = getCosmosGameContainer()->getPlayerFactory()->build();
    player->addTag(GameObjectTags::Player);

    player->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(center + glm::dvec3(dist * 1.03, 0.0, 0.0));
    player->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(velocity + 1000.0 * targetBody->calculateOrbitVelocity(dist * 0.03) * glm::dvec3(0.0, 1.0, 0.0));
    //testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(glm::dvec3(0.0));

    player->addComponent(new FocusComponent());

    addObject(player);

    getViewCamera()->setTarget(player);
    getViewCamera()->setStrategy(new CameraFirstPersonStrategy());
    getViewCamera()->setFov(66.0);

  //  debugMarker = new GameObject();
  //  auto model3d = getGameContainer()->getModel3dFactory()->build("debugrings.model3d.ini");
  //  auto drawableComponent = new AbsDrawableComponent(model3d, "debugrings.model3d.ini", glm::dvec3(0.0), glm::dquat(1.0, 0.0, 0.0, 0.0));
  //  debugMarker->addComponent(drawableComponent);
  //  debugMarker->addComponent(new Transformation3DComponent(0.0, center));
   // addObject(debugMarker);


    getCosmosGameContainer()->getCosmosRenderer()->getGalaxy()->update(center, 0.0);
}

PlayerMountState * FreeFlightGameStage::getPlayerMountState()
{
    return playerMountState;
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

void FreeFlightGameStage::onKeyDown(std::string key)
{
    if (key == "quick_save") {
        auto serializer = getCosmosGameContainer()->getSerializer();
        auto db = serializer->prepareNewDatabase("quick_save.db");
        serializer->serializeFreeFlightGameStage(db, this);
        serializer->closeDatabase(db);
    }

    if (key == "quick_load") {
        auto serializer = getCosmosGameContainer()->getSerializer();
        auto db = serializer->openDatabase("quick_save.db");
        auto newStage = serializer->deserializeFreeFlightGameStage(db);
        serializer->closeDatabase(db);
        getGameContainer()->registerStage("freeflight", newStage);
        getGameContainer()->setCurrentStage("freeflight");
    }

    // move this logic into state machine
    if (key == "enter_exit_vehicle") {
        if (playerMountState->getCurrentState() == "mounted") {

            playerMountState->getVehicle()->removeComponentsByType(ComponentTypes::Focus);
            auto vehicleTransformation = playerMountState->getVehicle()->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);

            auto player = getGameObjectsByTag(GameObjectTags::Player)[0];
            player->addComponent(new FocusComponent());
            auto playerTransformation = player->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);
            playerTransformation->setLinearVelocity(vehicleTransformation->getLinearVelocity());
            playerTransformation->setPosition(vehicleTransformation->getPosition());

            getViewCamera()->setTarget(player);
            getViewCamera()->setStrategy(new CameraFirstPersonStrategy());

            playerMountState->changeState("not-mounted");
        }
        else {
            auto player = getGameObjectsByTag(GameObjectTags::Player)[0];
            auto vehicles = getGameObjectsByTag(GameObjectTags::ControllableVehicle);
            if (vehicles.size() == 0) return;
            auto playerPos = player->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getPosition();
            double distance = 999999999999.0;
            auto choosenVehicle = vehicles[0];
            for (auto vehicle : vehicles) {
                auto vehiclePos = vehicle->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getPosition();
                double dst = glm::distance(vehiclePos, playerPos);
                if (dst < distance) {
                    choosenVehicle = vehicle;
                    distance = dst;
                }
            }

            getViewCamera()->setTarget(choosenVehicle);
            getViewCamera()->setStrategy(new CameraChaseStrategy(false));

            player->removeComponentsByType(ComponentTypes::Focus);
            choosenVehicle->addComponent(new FocusComponent);
            playerMountState->setVehicle(choosenVehicle);
            playerMountState->changeState("mounted");
        }
    }
}

void FreeFlightGameStage::onKeyUp(std::string key)
{
}

void FreeFlightGameStage::onUpdateObject(GameObject * object, double elapsed)
{
    auto cosmosRenderer = getCosmosGameContainer()->getCosmosRenderer();
    auto physicsComponent = object->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);

    // some stuff for drawings
    if (nullptr != physicsComponent) {
        //physicsComponent->setTimeScale(0.001);
        if (object->hasComponent(ComponentTypes::Focus)) {
            cosmosRenderer->setRaycastPoints({ physicsComponent->getPosition() });
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
             //   physicsComponent->setLinearVelocity(glm::mix(physicsComponent->getLinearVelocity(), airVelocity * 1000.0, elapsed * 2.0 * min(1.0, -dist * 1000.0)));
              //  physicsComponent->setLinearVelocity(physicsComponent->getLinearVelocity() + dirToShip * elapsed * 10.0 * min(1.0, -dist * 1000.0));
                //physicsComponent->applyAbsoluteImpulse(glm::dvec3(0.0), -physicsComponent->getLinearVelocity() * min(1.0, -dist * 1000.0) * 15000000.0 * elapsed);
                // physicsComponent->applyAbsoluteImpulse(glm::dvec3(0.0), dirToShip * 5000000.0 * min(1.0, -dist * 1000.0) * elapsed);
            }
            if (distatm < 0.0) {
            //    auto relativeVel = physicsComponent->getLinearVelocity() - airVelocity * 1000.0;
             //   physicsComponent->applyAbsoluteImpulse(glm::dvec3(0.0), relativeVel * elapsed * -1000.0);
            }
            auto closestPlanetRenderable = cosmosRenderer->getRenderableForCelestialBody(cosmosRenderer->getGalaxy()->getClosestPlanet());
            if (closestPlanetRenderable != nullptr) {
                auto res = closestPlanetRenderable->getRaycastResults(1)[0];
                starNameText->updateText("POINT: " + std::to_string(res.x) + "," + std::to_string(res.y) + "," + std::to_string(res.z) + ", DISTANCE: " + std::to_string(res.w * 100.0));
              //  debugMarker->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(getViewCamera()->getCamera()->getPosition() + glm::dvec3(res.x, res.y, res.z));
                if (res.w < 0.0) {
                    physicsComponent->setLinearVelocity(dirToShip * res.w + airVelocity);
                }
            }
        }
    }
}
