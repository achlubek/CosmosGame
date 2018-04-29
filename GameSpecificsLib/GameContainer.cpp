#include "stdafx.h"
#include "GameContainer.h"
#include "INIReader.h"
#include "VulkanToolkit.h"
#include "CosmosRenderer.h"
#include "AssetLoader.h"
#include "GameControls.h" 
#include "SQLiteDatabase.h"
#include "Model3dFactory.h"
#include "ModuleFactory.h"
#include "ShipFactory.h"
#include "GameObject.h"
#include "AbsComponent.h"
#include "AbsDrawableComponent.h"
#include "Transformation3DComponent.h"
#include "CameraController.h"
#include "GalaxyContainer.h"
#include "TimeProvider.h"
#include "UIRenderer.h"
#include "ModelsRenderer.h"
#include <ctype.h>

// kids, this is how to not do single responsibility principle
GameContainer::GameContainer()
    : AbsGameContainer()
{
    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(galaxydb);

    auto vulkanToolkit = getVulkanToolkit();
    cosmosRenderer = new CosmosRenderer(vulkanToolkit, getTimeProvider(), this, galaxy, getUIRenderer()->outputImage, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);
    cosmosRenderer->exposure = 0.0001;
    getModelsRenderer()->setRenderingScale(cosmosRenderer->scale);

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


    moduleFactory = new ModuleFactory(getModel3dFactory());
    shipFactory = new ShipFactory(getModel3dFactory(), moduleFactory);

    // fuck it for now

    // a test
    auto testship = shipFactory->build("Generix Explorer 1.ship_blueprint.ini");
    //auto testspawnpos = cosmosRenderer->galaxy->getAllStars()[666].getPosition(0);
    //auto testspawnradius = cosmosRenderer->galaxy->getAllStars()[666].radius;
   //cosmosRenderer->galaxy->update(testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getPosition());
    int targetStar = 1234;
    int targetPlanet = 4;
    int targetMoon = -1;
    GeneratedStarInfo star = galaxy->getAllStars()[targetStar - 1];
    auto center = star.getPosition(0);
    auto dist = star.radius;
    auto velocity = glm::dvec3(0.0);
    CelestialBody targetBody = CelestialBody();
    if (targetPlanet > 0) {
        galaxy->update(star.getPosition(0));
        auto planet = galaxy->getClosestStarPlanets()[targetPlanet - 1];
        center = planet.getPosition(0);
        dist = planet.radius - planet.fluidMaxLevel;
        velocity = planet.getLinearVelocity(getTimeProvider()->getTime());
        targetBody = planet;
        if (targetMoon > 0) {
            galaxy->update(planet.getPosition(0));
            auto moon = galaxy->getClosestPlanetMoons()[targetMoon - 1];
            center = moon.getPosition(0);
            dist = moon.radius - moon.fluidMaxLevel;
            velocity = moon.getLinearVelocity(getTimeProvider()->getTime());
            targetBody = moon;
        }
    }

    cosmosRenderer->galaxy->onClosestStarChange.add([&](GeneratedStarInfo star) {
        auto name = cosmosRenderer->galaxy->getStarName(star.starId);
        // if(name.length() > 0) name.at(0) = toupper(name.at(0));
        getCosmosRenderer()->updatingSafetyQueue.enqueue([=]() {
            starNameText->updateText("Star: " + std::to_string(star.starId) + " " + name);
        });
    });

    cosmosRenderer->galaxy->onClosestPlanetChange.add([&](CelestialBody body) {
        auto name = cosmosRenderer->galaxy->getCelestialBodyName(body.bodyId);
        printf(("\n\n" + std::to_string(body.bodyId) + "CHANGE!!" + name + "!!\n\n").c_str());
        // if (name.length() > 0) name.at(0) = toupper(name.at(0));
        getCosmosRenderer()->updatingSafetyQueue.enqueue([=]() {
            planetNameText->updateText("Planet: " + name);
        });
    });

    cosmosRenderer->galaxy->onClosestMoonChange.add([&](CelestialBody body) {
        auto name = cosmosRenderer->galaxy->getCelestialBodyName(body.bodyId);
        // if (name.length() > 0) name.at(0) = toupper(name.at(0));
        getCosmosRenderer()->updatingSafetyQueue.enqueue([=]() {
            moonNameText->updateText("Moon: " + name);
        });
    });

    getControls()->onKeyDown.add([&](std::string key) {
        if (key == "recompile_shaders") getCosmosRenderer()->recompileShaders(true);
    });

    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(center + glm::dvec3(0.0, dist * 1.03, 0.0));
    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(velocity + 1000.0 * targetBody.calculateOrbitVelocity(dist * 0.03) * glm::dvec3(1.0, 0.0, 0.0));

    addObject(testship);
    getViewCamera()->setTarget(testship);
}


GameContainer::~GameContainer()
{
}

ShipFactory * GameContainer::getShipFactory()
{
    return shipFactory;
}


CosmosRenderer * GameContainer::getCosmosRenderer()
{
    return cosmosRenderer;
}

GameContainer * GameContainer::getInstance()
{
    return static_cast<GameContainer*>(AbsGameContainer::getInstance());
}

void GameContainer::onDrawingStart()
{
    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStarsBuffer();
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->galaxy->update(getViewCamera()->getPosition());

        }
    });
    background1.detach();
}
void GameContainer::onDraw()
{
    cosmosRenderer->updateCameraBuffer(getViewCamera()->getInternalCamera(), getViewCamera()->getPosition());
    cosmosRenderer->draw();
}

void GameContainer::onUpdate(double elapsed)
{
}

void GameContainer::onUpdateObject(GameObject * object, double elapsed)
{
    auto physicsComponent = object->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);
    if (nullptr != physicsComponent) {
        physicsComponent->setTimeScale(0.001);
        auto g = cosmosRenderer->galaxy->getGravity(physicsComponent->getPosition(), getTimeProvider()->getTime());
        physicsComponent->applyGravity(g);
        gravityFluxText->updateText(std::to_string(glm::length(g)));
        altitudeText->updateText("Altitude KM: " + std::to_string(getCosmosRenderer()->galaxy->getClosestPlanet().getAltitude(physicsComponent->getPosition(), getTimeProvider()->getTime())));
        auto relativeVel = getCosmosRenderer()->galaxy->getClosestPlanet().getRelativeLinearVelocity(physicsComponent->getLinearVelocity(), getTimeProvider()->getTime());
        velocityText->updateText("Relative velocity M/S: " + std::to_string(1000.0 * glm::length(relativeVel)));
    }
}
