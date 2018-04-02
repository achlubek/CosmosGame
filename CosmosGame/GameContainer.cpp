#include "stdafx.h"
#include "GameContainer.h"
#include "INIReader.h"
#include "VulkanToolkit.h"
#include "CosmosRenderer.h"
#include "AssetLoader.h"
#include "GameControls.h" 
#include "SQLiteDatabase.h"
#include "ShipFactory.h"
#include "GameObject.h"
#include "AbsComponent.h"
#include "AbsDrawableComponent.h"
#include "Transformation3DComponent.h"
#include "CameraController.h"
#include "GalaxyContainer.h"
#include "TimeProvider.h"
#include "UIRenderer.h"
#include <ctype.h>

GameContainer* GameContainer::instance = nullptr;

// kids, this is how to not do single responsibility principle
GameContainer::GameContainer()
{
    instance = this;
    activeObjects = {};

    timeProvider = new TimeProvider();

    auto galaxydb = new SQLiteDatabase("galaxy.db");
    auto galaxy = new GalaxyContainer();
    galaxy->loadFromDatabase(galaxydb);

    INIReader* configreader = new INIReader("settings.ini");
    vulkanToolkit = new VulkanToolkit();
    vulkanToolkit->initialize(configreader->geti("window_width"), configreader->geti("window_height"), true, "Galaxy Game");

    Mouse* mouse = new Mouse(vulkanToolkit->window);
    Keyboard* keyboard = new Keyboard(vulkanToolkit->window);

    ui = new UIRenderer(vulkanToolkit, mouse, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);

    cosmosRenderer = new CosmosRenderer(vulkanToolkit, timeProvider, this, galaxy, ui->outputImage, vulkanToolkit->windowWidth, vulkanToolkit->windowHeight);
    cosmosRenderer->exposure = 0.0001;

    fpsText = new UIText(ui, 0.01, 0.01, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("chintzy.ttf"), 16, "Hmm");
    ui->addDrawable(fpsText);

    gravityFluxText = new UIText(ui, 0.01, 0.11, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("chintzy.ttf"), 16, "Hmm");
    ui->addDrawable(gravityFluxText);

    starNameText = new UIText(ui, 0.01, 0.21, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("chintzy.ttf"), 16, "Hmm");
    ui->addDrawable(starNameText);

    planetNameText = new UIText(ui, 0.01, 0.31, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("chintzy.ttf"), 16, "Hmm");
    ui->addDrawable(planetNameText);

    moonNameText = new UIText(ui, 0.01, 0.41, UIColor(1.0, 1.0, 1.0, 1.0), Media::getPath("chintzy.ttf"), 16, "Hmm");
    ui->addDrawable(moonNameText);

    assetLoader = new AssetLoader(vulkanToolkit);

    gameControls = new GameControls(keyboard, mouse, "controls.ini");
     
    gameControls->onKeyDown.add([&](std::string key) {
        if (key == "recompile_shaders") getCosmosRenderer()->recompileShaders(true);
    });

    database = new SQLiteDatabase(Media::getPath("gamedata.db"));

    shipFactory = new ShipFactory();

    // fuck it for now
    viewCamera = new CameraController();

    // a test
    auto testship = shipFactory->build(1);
    //auto testspawnpos = cosmosRenderer->galaxy->getAllStars()[666].getPosition(0);
    //auto testspawnradius = cosmosRenderer->galaxy->getAllStars()[666].radius;
   //cosmosRenderer->galaxy->update(testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->getPosition());
    int targetStar = 666;
    int targetPlanet = 1;
    int targetMoon = -1;
    GeneratedStarInfo star = galaxy->getAllStars()[targetStar - 1];
    auto center = star.getPosition(0);
    auto dist = star.radius;
    auto velocity = glm::vec3(0.0);
    CelestialBody targetBody = CelestialBody();
    if (targetPlanet > 0) {
        galaxy->update(star.getPosition(0));
        auto planet = galaxy->getClosestStarPlanets()[targetPlanet - 1];
        center = planet.getPosition(0);
        dist = planet.radius - planet.fluidMaxLevel;
        velocity = planet.getLinearVelocity(timeProvider->getTime());
        targetBody = planet;
        if (targetMoon > 0) {
            galaxy->update(planet.getPosition(0));
            auto moon = galaxy->getClosestPlanetMoons()[targetMoon - 1];
            center = moon.getPosition(0);
            dist = moon.radius - moon.fluidMaxLevel;
            velocity = moon.getLinearVelocity(timeProvider->getTime());
            targetBody = moon;
        }
    }

    galaxy->onClosestStarChange.add([&](GeneratedStarInfo star) {
        auto name = galaxy->getStarName(star.starId);
        // if(name.length() > 0) name.at(0) = toupper(name.at(0));
        starNameText->updateText("Star: " + std::to_string(star.starId) + " " + name);
    });

    galaxy->onClosestPlanetChange.add([&](CelestialBody body) {
        auto name = galaxy->getCelestialBodyName(body.bodyId);
        printf(("\n\n" + std::to_string(body.bodyId) + "CHANGE!!" + name + "!!\n\n").c_str());
        // if (name.length() > 0) name.at(0) = toupper(name.at(0));
        planetNameText->updateText("Planet: " + name);
    });

    galaxy->onClosestMoonChange.add([&](CelestialBody body) {
        auto name = galaxy->getCelestialBodyName(body.bodyId);
        // if (name.length() > 0) name.at(0) = toupper(name.at(0));
        moonNameText->updateText("Moon: " + name);
    });

    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(center + glm::dvec3(0.0, dist * 3.0, -dist * 3.0));
    testship->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setLinearVelocity(velocity);

    activeObjects.push_back(testship);
    viewCamera->setTarget(activeObjects[0]);
}


GameContainer::~GameContainer()
{
}

ShipFactory * GameContainer::getShipFactory()
{
    return shipFactory;
}

void GameContainer::addObject(GameObject * object)
{
    activeObjects.push_back(object);
}

void GameContainer::removeObject(GameObject * object)
{
    auto found = std::find(activeObjects.begin(), activeObjects.end(), object);

    if (found != activeObjects.end()) {
        activeObjects.erase(found);
    }
}

void GameContainer::removeAllObjects()
{
    activeObjects.clear();
}

void GameContainer::updateObjects()
{
    double nowtime = glfwGetTime();
    for (int i = 0; i < activeObjects.size(); i++) {
        auto physicsComponent = activeObjects[i]->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D);
        if (nullptr != physicsComponent) {
            auto g = cosmosRenderer->galaxy->getGravity(physicsComponent->getPosition(), timeProvider->getTime());
            physicsComponent->applyGravity(g);
            gravityFluxText->updateText(std::to_string(glm::length(g)));
        }
        activeObjects[i]->update(nowtime - lastTime);
    }
    viewCamera->update(nowtime - lastTime);
    timeProvider->update(nowtime - lastTime);
    lastTime = nowtime;
}

void GameContainer::drawDrawableObjects(VulkanRenderStage* stage, VulkanDescriptorSet* set)
{
    auto observerPosition = viewCamera->getPosition();
    for (int i = 0; i < activeObjects.size(); i++) {
        auto comps = activeObjects[i]->getAllComponents();
        for (int g = 0; g < comps.size(); g++) {
            if (comps[g]->isDrawable()) {
                auto drawable = static_cast<AbsDrawableComponent*>(comps[g]);
                drawable->draw(observerPosition, stage, set);
            }
        }
    }
}

GameContainer * GameContainer::getInstance()
{
    return instance;
}

CosmosRenderer * GameContainer::getCosmosRenderer()
{
    return cosmosRenderer;
}

VulkanToolkit * GameContainer::getVulkanToolkit()
{
    return vulkanToolkit;
}

SQLiteDatabase * GameContainer::getDatabase()
{
    return database;
}

GameControls * GameContainer::getControls()
{
    return gameControls;
}

TimeProvider * GameContainer::getTimeProvider()
{
    return nullptr;
}

glm::vec2 GameContainer::getResolution()
{
    return glm::vec2((float)vulkanToolkit->windowWidth, (float)vulkanToolkit->windowHeight);
}

void GameContainer::startGameLoops()
{
    cosmosRenderer->mapBuffers();
    cosmosRenderer->updateStarsBuffer();
    std::thread background1 = std::thread([&]() {
        while (true) {
            cosmosRenderer->galaxy->update(viewCamera->getPosition());

        }
    });
    background1.detach();
    int frames = 0;
    double lastTime = 0.0;
    double lastRawTime = 0.0;
    while (!vulkanToolkit->shouldCloseWindow()) {
        frames++;
        double time = glfwGetTime();
        double nowtime = floor(time);
        if (nowtime != lastTime) {
            printf("FPS %d\n", frames);
            fpsText->updateText("FPS: " + std::to_string(frames));
            frames = 0;
        }
        double elapsed_x100 = (float)(100.0 * (time - lastRawTime));
        double elapsed = (float)((time - lastRawTime));
        lastRawTime = time;
        lastTime = nowtime;

        //
        ui->draw();
        cosmosRenderer->updateCameraBuffer(viewCamera->getInternalCamera(), viewCamera->getPosition());
        cosmosRenderer->draw();

        updateObjects();

        vulkanToolkit->poolEvents();
    }
    cosmosRenderer->unmapBuffers();
}
