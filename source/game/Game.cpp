///------------------------------------------------------------------------------------------------
///  Game.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "../engine/input/components/InputStateSingletonComponent.h"
#include "../engine/input/utils/InputUtils.h"
#include "../engine/rendering/components/CameraSingletonComponent.h"
#include "../engine/rendering/utils/ModelUtils.h"

///------------------------------------------------------------------------------------------------

void Game::VOnInit(genesis::ecs::World& world)
{
    genesis::rendering::LoadAndCreateModelByName("monkey", glm::vec3(0.0f, 0.0f, 0.0f), world);
}

///------------------------------------------------------------------------------------------------

void Game::VOnUpdate(const float dt, genesis::ecs::World& world)
{
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    float moveSpeed = 5.0f;
    //float lookSpeed = 10.0f;

    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_UP, world))
    {
        
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_DOWN, world))
    {
        
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_LEFT, world))
    {
        cameraComponent.mPosition += dt * moveSpeed * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_RIGHT, world))
    {
        cameraComponent.mPosition -= dt * moveSpeed * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_FORWARD, world))
    {
        cameraComponent.mPosition += dt * moveSpeed * cameraComponent.mFrontVector;
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_BACKWARD, world))
    {
        cameraComponent.mPosition -= dt * moveSpeed * cameraComponent.mFrontVector;
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_LOOK_UP, world))
    {
        
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_LOOK_DOWN, world))
    {
        
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_LOOK_LEFT, world))
    {
        
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_LOOK_RIGHT, world))
    {
        
    }
}

///------------------------------------------------------------------------------------------------