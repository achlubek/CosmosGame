#include "stdafx.h"
#include "ModelPreviewGameStage.h"
#include "Transformation3DComponent.h"
#include "ThrustGeneratorComponent.h"
#include "Star.h"
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
#include "CameraOrbitStrategy.h"

ModelPreviewGameStage::ModelPreviewGameStage(AbsGameContainer* container)
    : AbsGameStage(container)
{
    int targetStar = 676;
    int targetPlanet = 3;
    int targetMoon = 0;
    auto galaxy = getCosmosGameContainer()->getCosmosRenderer()->getGalaxy();
    getCosmosGameContainer()->getCosmosRenderer()->setExposure(0.00002);

    auto targetBody = galaxy->getByPath(targetStar, targetPlanet, targetMoon);
    auto center = targetBody->getPosition(0);
    auto dist = targetBody->radius;

    viewCenter = center + glm::dvec3(0.0, dist * 1.043, 0.0);
}


ModelPreviewGameStage::~ModelPreviewGameStage()
{
}

void ModelPreviewGameStage::addPreviewObject(GameObject * object)
{
    object->getComponent<Transformation3DComponent>(ComponentTypes::Transformation3D)->setPosition(viewCenter);
    addObject(object);
    getViewCamera()->setTarget(object);
    getViewCamera()->setStrategy(new CameraChaseStrategy(false));
    getViewCamera()->setFov(66.0);
}

GameContainer * ModelPreviewGameStage::getCosmosGameContainer()
{
    return static_cast<GameContainer*>(getGameContainer());
}

void ModelPreviewGameStage::onSwitchTo()
{

}
void ModelPreviewGameStage::onSwitchFrom()
{

}
void ModelPreviewGameStage::onDraw()
{
}

void ModelPreviewGameStage::onUpdate(double elapsed)
{
}

void ModelPreviewGameStage::onUpdateObject(GameObject * object, double elapsed)
{
}