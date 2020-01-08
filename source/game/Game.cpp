///------------------------------------------------------------------------------------------------
///  Game.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "NativeFunctionBinder.h"
#include "../engine/ECS.h"
#include "../engine/debug/components/ConsoleStateSingletonComponent.h"
#include "../engine/debug/systems/ConsoleManagementSystem.h"
#include "../engine/input/components/InputStateSingletonComponent.h"
#include "../engine/input/utils/InputUtils.h"
#include "../engine/input/systems/RawInputHandlingSystem.h"
#include "../engine/rendering/components/CameraSingletonComponent.h"
#include "../engine/rendering/utils/ModelUtils.h"
#include "../engine/rendering/systems/AnimationSystem.h"
#include "../engine/rendering/systems/RenderingSystem.h"
#include "../engine/scripting/components/ScriptComponent.h"
#include "../engine/scripting/service/LuaScriptingService.h"
#include "../engine/scripting/systems/ScriptingSystem.h"

///------------------------------------------------------------------------------------------------

void Game::VOnSystemsInit()
{
    genesis::ecs::World::GetInstance().AddSystem(std::make_unique<genesis::input::RawInputHandlingSystem>());
    genesis::ecs::World::GetInstance().AddSystem(std::make_unique<genesis::rendering::AnimationSystem>());
    genesis::ecs::World::GetInstance().AddSystem(std::make_unique<genesis::scripting::ScriptingSystem>());

#ifndef NDEBUG
    genesis::ecs::World::GetInstance().AddSystem(std::make_unique<genesis::debug::ConsoleManagementSystem>());
#endif

    genesis::ecs::World::GetInstance().AddSystem(std::make_unique<genesis::rendering::RenderingSystem>());

    NativeFunctionBinder::BindNativeFunctionsToLua();
}

///------------------------------------------------------------------------------------------------

void Game::VOnGameInit()
{
    const auto monkeyEntityId = genesis::rendering::LoadAndCreateModelByName("monkey", glm::vec3(0.1f, 0.2f, 0.3f), genesis::ecs::World::GetInstance(), StringId("monkey"));    

    auto scriptComponent = std::make_unique<genesis::scripting::ScriptComponent>();
    scriptComponent->mScriptName = StringId("test");
    scriptComponent->mScriptType = genesis::scripting::ScriptType::CONTINUOUS_EXECUTION;
    genesis::ecs::World::GetInstance().AddComponent<genesis::scripting::ScriptComponent>(monkeyEntityId, std::move(scriptComponent));

    genesis::rendering::LoadAndCreateGuiSprite("gui_base", "debug_square", StringId("console"), glm::vec3(0.0f, 0.0f, 0.0f), genesis::ecs::World::GetInstance(), StringId("console_background"));
}

///------------------------------------------------------------------------------------------------

void Game::VOnUpdate(const float dt)
{
    auto& world = genesis::ecs::World::GetInstance();

    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    float moveSpeed = 5.0f;
    float lookSpeed = 1.0f;
    float zoomSpeed = 0.2f;

    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_UP, world))
    {
        cameraComponent.mPosition.y += moveSpeed * dt;
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_DOWN, world))
    {
        cameraComponent.mPosition.y -= moveSpeed * dt;
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
        cameraComponent.mPitch += lookSpeed * dt;
        if (cameraComponent.mPitch >= 2 * genesis::math::PI)
        {
            cameraComponent.mPitch = cameraComponent.mPitch - 2 * genesis::math::PI;
        }
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_LOOK_DOWN, world))
    {
        cameraComponent.mPitch -= lookSpeed * dt;
        if (cameraComponent.mPitch <= 0.0f)
        {
            cameraComponent.mPitch = 2 * genesis::math::PI + cameraComponent.mPitch;
        }
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_LOOK_LEFT, world))
    {
        cameraComponent.mYaw += lookSpeed * dt;
        if (cameraComponent.mYaw >= 2 * genesis::math::PI)
        {
            cameraComponent.mYaw = cameraComponent.mYaw - 2 * genesis::math::PI;
        }        
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_LOOK_RIGHT, world))
    {
        cameraComponent.mYaw -= lookSpeed * dt;
        if (cameraComponent.mYaw <= 0.0f)
        {
            cameraComponent.mYaw = 2 * genesis::math::PI + cameraComponent.mYaw;
        }
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_ZOOM_IN, world))
    {    
        cameraComponent.mFieldOfView -= zoomSpeed * dt;
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_ZOOM_OUT, world))
    {
        cameraComponent.mFieldOfView += zoomSpeed * dt;
    }
    if (genesis::input::IsActionTypeKeyTapped(genesis::input::InputActionType::CONSOLE_TOGGLE, world))
    {
        auto& consoleStateComponent = genesis::ecs::World::GetInstance().GetSingletonComponent<genesis::debug::ConsoleStateSingletonComponent>();
        consoleStateComponent.mEnabled = !consoleStateComponent.mEnabled;
    }

    cameraComponent.mFrontVector.x = genesis::math::Cosf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.y = genesis::math::Sinf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.z = genesis::math::Sinf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
}

///------------------------------------------------------------------------------------------------