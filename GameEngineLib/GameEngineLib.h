#pragma once
#include "BatteryDrainer.h"
#include "Camera/AbsCameraViewStrategy.h"
#include "Camera/CameraChaseStrategy.h"
#include "Camera/CameraController.h"
#include "Camera/CameraFirstPersonStrategy.h"
#include "Camera/ManualCameraStrategy.h"
#include "Camera/PointerDrivenCameraStrategy.h"
#include "GameContainer.h"
#include "GameControls.h"
#include "GameEngineLib.h"
#include "GameObject.h"
#include "GameObjectTags.h"
#include "GameStageCollection.h"
#include "json.h"
#include "Model3dFactory.h"
#include "ModuleFactory.h"
#include "PlayerFactory.h"
#include "ShipFactory.h"
#include "StateMachines/AbsStateMachine.h"
#include "StateMachines/PlayerMountState.h"
#include "StateMachines/ShipOrientationControllerState.h"
#include "TimeProvider.h"

#include "GameStages/AbsGameStage.h"
#include "GameStages/CelestialBodyPreviewGameStage.h"
#include "GameStages/FreeFlightGameStage.h"
#include "GameStages/MainMenuGameStage.h"
#include "GameStages/ModelPreviewGameStage.h"

#include "Serializer.h"

#include "Components/AbsComponent.h"
#include "Components/AbsDrawableComponent.h"
#include "Components/BatteryComponent.h"
#include "Components/ComponentTypes.h"
#include "Components/FocusComponent.h"
#include "Components/PlayerManualControlsComponent.h"
#include "Components/ShipAutopilotComponent.h"
#include "Components/ShipManualControlsComponent.h"
#include "Components/ThrustControllerComponent.h"
#include "Components/ThrustGeneratorComponent.h"
#include "Components/Transformation3DComponent.h"


#define LOG(a) GameContainer::getInstance()->getLogger()->log(LogSeverity::Normal, a);
#define LOGERR(a) GameContainer::getInstance()->getLogger()->log(LogSeverity::Important, a);
#define LOGINFO(a) GameContainer::getInstance()->getLogger()->log(LogSeverity::Trivial, a);
#define LOGVERBOSE(a) GameContainer::getInstance()->getLogger()->log(LogSeverity::Trivial, a);