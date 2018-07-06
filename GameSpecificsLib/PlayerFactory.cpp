#include "stdafx.h"
#include "PlayerFactory.h"
#include "GameObject.h"
#include "Transformation3DComponent.h"
#include "PlayerManualControlsComponent.h"

PlayerFactory::PlayerFactory()
{
}


PlayerFactory::~PlayerFactory()
{
}

GameObject * PlayerFactory::build()
{
    GameObject* player = new GameObject();

    auto transformComponent = new Transformation3DComponent(100.0, glm::dvec3(0.0));
    player->addComponent(transformComponent);

    player->addComponent(new PlayerManualControlsComponent());

    return player;
}
