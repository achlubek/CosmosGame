#pragma once

#include "Camera/AbsCameraViewStrategy.h"
#include "Camera/CameraChaseStrategy.h"
#include "Camera/CameraController.h"
#include "Camera/CameraFirstPersonStrategy.h"
#include "Camera/ManualCameraStrategy.h"
#include "Camera/PointerDrivenCameraStrategy.h"

#include "Events/OnKeyPressEvent.h"
#include "Events/OnKeyReleaseEvent.h"

#include "Core/IGlobalService.h"
#include "Core/GameContainer.h"
#include "Core/GameControls.h"
#include "Core/GameObject.h"
#include "Core/GameObjectTags.h"
#include "Core/GameStageCollection.h"
#include "Core/TimeProvider.h"

#include "StateMachines/AbsStateMachine.h"
#include "StateMachines/PlayerMountState.h"
#include "StateMachines/ShipOrientationControllerState.h"

#include "Components/ComponentTypes.h"
#include "Components/AbsComponent.h"
#include "Components/AbsDrawableComponent.h"
#include "Components/BatteryComponent.h"
#include "Components/FocusComponent.h"
#include "Components/PlayerManualControlsComponent.h"
#include "Components/ShipAutopilotComponent.h"
#include "Components/ShipManualControlsComponent.h"
#include "Components/ThrustControllerComponent.h"
#include "Components/ThrustGeneratorComponent.h"
#include "Components/Transformation3DComponent.h"

#include "GameStages/AbsGameStage.h"
#include "GameStages/CelestialBodyPreviewGameStage.h"
#include "GameStages/FreeFlightGameStage.h"
#include "GameStages/MainMenuGameStage.h"
#include "GameStages/ModelPreviewGameStage.h"

#include "Utilities/BatteryDrainer.h"
#include "Utilities/Serializer.h"

#include "Factories/Model3dFactory.h"
#include "Factories/ModuleFactory.h"
#include "Factories/PlayerFactory.h"
#include "Factories/ShipFactory.h"

#define LOG(a) GameContainer::getInstance()->getLogger()->log(LogSeverity::Normal, a);
#define LOGERR(a) GameContainer::getInstance()->getLogger()->log(LogSeverity::Important, a);
#define LOGINFO(a) GameContainer::getInstance()->getLogger()->log(LogSeverity::Trivial, a);
#define LOGVERBOSE(a) GameContainer::getInstance()->getLogger()->log(LogSeverity::Trivial, a);