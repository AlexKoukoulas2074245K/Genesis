///------------------------------------------------------------------------------------------------
///  Game.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "physics/systems/PhysicsMovementApplicationSystem.h"
#include "physics/systems/PhysicsCollisionDetectionSystem.h"
#include "physics/systems/PhysicsCollisionResponseSystem.h"
#include "scene/systems/SceneUpdaterSystem.h"
#include "../engine/ECS.h"
#include "../engine/common/components/TransformComponent.h"
#include "../engine/common/utils/Logging.h"
#include "../engine/common/utils/MathUtils.h"
#include "../engine/debug/components/DebugViewStateSingletonComponent.h"
#include "../engine/debug/systems/ConsoleManagementSystem.h"
#include "../engine/debug/systems/DebugViewManagementSystem.h"
#include "../engine/debug/utils/ConsoleCommandUtils.h"
#include "../engine/input/components/InputStateSingletonComponent.h"
#include "../engine/input/utils/InputUtils.h"
#include "../engine/input/systems/RawInputHandlingSystem.h"
#include "../engine/rendering/components/CameraSingletonComponent.h"
#include "../engine/rendering/components/LightStoreSingletonComponent.h"
#include "../engine/rendering/components/RenderableComponent.h"
#include "../engine/rendering/utils/FontUtils.h"
#include "../engine/rendering/utils/LightUtils.h"
#include "../engine/rendering/utils/MeshUtils.h"
#include "../engine/rendering/systems/RenderingSystem.h"
#include "../engine/resources/MeshResource.h"
#include "../engine/resources/ResourceLoadingService.h"
#include "../engine/scripting/components/ScriptComponent.h"
#include "../engine/scripting/service/LuaScriptingService.h"
#include "../engine/scripting/systems/ScriptingSystem.h"

///------------------------------------------------------------------------------------------------

void Game::VOnSystemsInit()
{
    auto& world = genesis::ecs::World::GetInstance();
    world.AddSystem(std::make_unique<genesis::input::RawInputHandlingSystem>());
    world.AddSystem(std::make_unique<genesis::scripting::ScriptingSystem>());

#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
    world.AddSystem(std::make_unique<genesis::debug::ConsoleManagementSystem>());
    world.AddSystem(std::make_unique<genesis::debug::DebugViewManagementSystem>());
#endif
    
    world.AddSystem(std::make_unique<physics::PhysicsMovementApplicationSystem>());
    world.AddSystem(std::make_unique<scene::SceneUpdaterSystem>());
    world.AddSystem(std::make_unique<physics::PhysicsCollisionDetectionSystem>());
    world.AddSystem(std::make_unique<physics::PhysicsCollisionResponseSystem>());
    
    world.AddSystem(std::make_unique<genesis::rendering::RenderingSystem>());
}

///------------------------------------------------------------------------------------------------

static void CreateSphereAtRandomPosition(const int i)
{
    const auto sphereEntityId = genesis::rendering::LoadAndCreateModelByName
    (
        "sphere",
        glm::vec3(0.0f - i * 0.5f, 0.0f + i * 0.5f, 0.12f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.3f, 0.3f, 0.3f),
        StringId("sphere")
    );
    
    auto& world = genesis::ecs::World::GetInstance();
    
    auto& transformComponent = world.GetComponent<genesis::TransformComponent>(sphereEntityId);
    transformComponent.mRotation.y = genesis::math::RandomFloat(0.0f, genesis::math::PI);
    
    auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(sphereEntityId);
    auto& resource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceId);
    
    renderableComponent.mMaterial.mAmbient   = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    renderableComponent.mMaterial.mDiffuse   = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
    renderableComponent.mMaterial.mSpecular  = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    renderableComponent.mMaterial.mShininess = 32.0f;
    renderableComponent.mIsAffectedByLight   = true;
    
    auto physicsComponent = std::make_unique<physics::PhysicsComponent>();
    physicsComponent->mCollidableDimensions = transformComponent.mScale * resource.GetDimensions();
    physicsComponent->mDirection = glm::vec3(genesis::math::RandomFloat(-1.0f, 1.0f), genesis::math::RandomFloat(-1.0f, 1.0f), 0.0f);
    physicsComponent->mDirection = glm::normalize(physicsComponent->mDirection);
    //physicsComponent->mVelocitySpeed = 0.2f;
    //physicsComponent->mRotationalSpeed = 0.3f;
    
    world.AddComponent<physics::PhysicsComponent>(sphereEntityId, std::move(physicsComponent));
}

void Game::VOnGameInit()
{
    RegisterConsoleCommands();
    for (int i = 0; i < 2; ++i)
    {
        CreateSphereAtRandomPosition(i);
    }
    
    genesis::rendering::AddLightSource(glm::vec3(0.0f, 0.0f, 1.0f));
    genesis::rendering::AddLightSource(glm::vec3(0.0f, 4.0f, 0.0f));
    genesis::rendering::AddLightSource(glm::vec3(2.0f, 2.0f, 0.0f));
}

///------------------------------------------------------------------------------------------------

static float dtAccum = 0.0f;

void Game::VOnUpdate(const float dt)
{
    auto& world = genesis::ecs::World::GetInstance();

    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    auto& lightStoreComponent = world.GetSingletonComponent<genesis::rendering::LightStoreSingletonComponent>();
    
    dtAccum += dt;
    lightStoreComponent.mLightPositions[0].x = genesis::math::Sinf(dtAccum/2) * 2;
    lightStoreComponent.mLightPositions[0].z = genesis::math::Cosf(dtAccum/2) * 2;
    
    float moveSpeed = 5.0f;
    float lookSpeed = 1.0f;
    //float zoomSpeed = 0.2f;

    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_UP))
    {
        cameraComponent.mPosition.y += moveSpeed * dt;
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_DOWN))
    {
        cameraComponent.mPosition.y -= moveSpeed * dt;
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_LEFT))
    {
        cameraComponent.mPosition += dt * moveSpeed * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_RIGHT))
    {
        cameraComponent.mPosition -= dt * moveSpeed * glm::normalize(glm::cross(cameraComponent.mFrontVector, cameraComponent.mUpVector));
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_FORWARD))
    {
        cameraComponent.mPosition += dt * moveSpeed * cameraComponent.mFrontVector;
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::CAMERA_MOVE_BACKWARD))
    {
        cameraComponent.mPosition -= dt * moveSpeed * cameraComponent.mFrontVector;
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::UP_ARROW_KEY))
    {
        cameraComponent.mPitch += lookSpeed * dt;
        if (cameraComponent.mPitch >= 2 * genesis::math::PI)
        {
            cameraComponent.mPitch = cameraComponent.mPitch - 2 * genesis::math::PI;
        }
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::DOWN_ARROW_KEY))
    {
        cameraComponent.mPitch -= lookSpeed * dt;
        if (cameraComponent.mPitch <= 0.0f)
        {
            cameraComponent.mPitch = 2 * genesis::math::PI + cameraComponent.mPitch;
        }
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::LEFT_ARROW_KEY))
    {
        cameraComponent.mYaw += lookSpeed * dt;
        if (cameraComponent.mYaw >= 2 * genesis::math::PI)
        {
            cameraComponent.mYaw = cameraComponent.mYaw - 2 * genesis::math::PI;
        }        
    }
    if (genesis::input::IsActionTypeKeyPressed(genesis::input::InputActionType::RIGHT_ARROW_KEY))
    {
        cameraComponent.mYaw -= lookSpeed * dt;
        if (cameraComponent.mYaw <= 0.0f)
        {
            cameraComponent.mYaw = 2 * genesis::math::PI + cameraComponent.mYaw;
        }
    }
    if (genesis::input::IsActionTypeKeyTapped(genesis::input::InputActionType::CAMERA_ZOOM_IN))
    {
        auto sphereEntity = world.FindEntityWithName(StringId("sphere"));
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(sphereEntity);
        renderableComponent.mMaterial.mShininess -= 1.0f;
    }
    if (genesis::input::IsActionTypeKeyTapped(genesis::input::InputActionType::CAMERA_ZOOM_OUT))
    {
        auto sphereEntity = world.FindEntityWithName(StringId("sphere"));
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(sphereEntity);
        renderableComponent.mMaterial.mShininess += 1.0f;
    }      

    cameraComponent.mFrontVector.x = genesis::math::Cosf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.y = genesis::math::Sinf(cameraComponent.mPitch);
    cameraComponent.mFrontVector.z = genesis::math::Sinf(cameraComponent.mYaw) * genesis::math::Cosf(cameraComponent.mPitch);
}

///------------------------------------------------------------------------------------------------

void Game::RegisterConsoleCommands() const
{
#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
    genesis::debug::RegisterConsoleCommand(StringId("scene_debug"), [](const std::vector<std::string>& commandTextComponents)
    {
        static const std::unordered_set<std::string> sAllowedOptions = { "on", "off" };

        const std::string USAGE_STRING = "Usage: scene_debug on|off";

        if (commandTextComponents.size() != 2 || sAllowedOptions.count(StringToLower(commandTextComponents[1])) == 0)
        {
            return genesis::debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        const auto& world = genesis::ecs::World::GetInstance();
        auto& debugViewStateComponent = world.GetSingletonComponent<genesis::debug::DebugViewStateSingletonComponent>();

        debugViewStateComponent.mSceneGraphDisplayEnabled = StringToLower(commandTextComponents[1]) == "on";

        return genesis::debug::ConsoleCommandResult(true);
    });
#endif
}
