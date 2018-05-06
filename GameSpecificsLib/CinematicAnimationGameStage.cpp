#include "stdafx.h"
#include "CinematicAnimationGameStage.h"
#include "AbsGameContainer.h"
#include "GameContainer.h"
#include "AbsGameStage.h"
#include "Interpolator.h"
#include "PointerDrivenCameraStrategy.h"
#include "CameraController.h"
#include "CinematicScenarioReader.h"
#include "TimeProvider.h"


CinematicAnimationGameStage::CinematicAnimationGameStage(AbsGameContainer* container, std::string imediaKey, std::string iswitchToStage)
    : AbsGameStage(container), mediaKey(imediaKey), pointerDrivenCamera(new PointerDrivenCameraStrategy()), switchToStage(iswitchToStage)
{

}

CinematicAnimationGameStage::~CinematicAnimationGameStage()
{

}

void CinematicAnimationGameStage::onDraw()
{

}

void CinematicAnimationGameStage::onSwitchTo()
{
    getGameContainer()->getInterpolator()->removeAllByTag(mediaKey);
    getViewCamera()->setStrategy(pointerDrivenCamera);
    auto container = static_cast<GameContainer*>(getGameContainer());
    auto scenario = CinematicScenarioReader(getGameContainer()->getInterpolator(), container->getCosmosRenderer(), pointerDrivenCamera, getTimeProvider());
    scenario.load(mediaKey, getTimeProvider()->getTime());
    animationDuration = scenario.getAnimationDuration();
    started = true;
    scenario.execute();
}

void CinematicAnimationGameStage::onSwitchFrom()
{
    GameContainer::getInstance()->getInterpolator()->removeAllByTag(mediaKey);
}

void CinematicAnimationGameStage::onUpdate(double elapsed)
{
    if (!started) return;
    animationDuration -= elapsed;
    if (animationDuration <= 0.0) {
        animationDuration = 0.0;
        GameContainer::getInstance()->setCurrentStage(switchToStage);
    }
}

void CinematicAnimationGameStage::onUpdateObject(GameObject * object, double elapsed)
{

}