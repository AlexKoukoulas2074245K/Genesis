///------------------------------------------------------------------------------------------------
///  PhysicsMovementApplicationSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#include "PhysicsMovementApplicationSystem.h"
#include "../../scene/components/SceneStateSingletonComponent.h"
#include "../../scene/scenegraphs/ISceneGraph.h"

///-----------------------------------------------------------------------------------------------

namespace physics
{

///-----------------------------------------------------------------------------------------------

PhysicsMovementApplicationSystem::PhysicsMovementApplicationSystem()
{
}

///-----------------------------------------------------------------------------------------------

void PhysicsMovementApplicationSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& sceneStateComponent   = genesis::ecs::World::GetInstance().GetSingletonComponent<scene::SceneStateSingletonComponent>();
    const auto& sceneOrigin     = sceneStateComponent.mSceneGraph->VGetSceneOrigin();
    const auto& sceneDimensions = sceneStateComponent.mSceneGraph->VGetSceneDimensions();
    
    const auto sceneLeftBound   = sceneOrigin.x - sceneDimensions.x * 0.5f;
    const auto sceneRightBound  = sceneOrigin.x + sceneDimensions.x * 0.5f;
    const auto sceneTopBound    = sceneOrigin.y + sceneDimensions.y * 0.5f;
    const auto sceneBottomBound = sceneOrigin.y - sceneDimensions.y * 0.5f;
    
    for (const auto& entityId: entitiesToProcess)
    {
        auto& physicsComponent = genesis::ecs::World::GetInstance().GetComponent<PhysicsComponent>(entityId);
        auto& transformComponent = genesis::ecs::World::GetInstance().GetComponent<genesis::TransformComponent>(entityId);
        
        transformComponent.mPosition += physicsComponent.mDirection * physicsComponent.mVelocitySpeed * dt;
        
        if (transformComponent.mPosition.x - physicsComponent.mCollidableDimensions.x * 0.5f < sceneLeftBound)
        {
            transformComponent.mPosition.x = sceneLeftBound + physicsComponent.mCollidableDimensions.x * 0.5f;
            physicsComponent.mDirection.x *= -1.0f;
        }
        else if (transformComponent.mPosition.x + physicsComponent.mCollidableDimensions.x * 0.5f > sceneRightBound)
        {
            transformComponent.mPosition.x = sceneRightBound - physicsComponent.mCollidableDimensions.x * 0.5f;
            physicsComponent.mDirection.x *= -1.0f;
        }
        else if (transformComponent.mPosition.y + physicsComponent.mCollidableDimensions.y * 0.5f > sceneTopBound)
        {
            transformComponent.mPosition.y = sceneTopBound - physicsComponent.mCollidableDimensions.y * 0.5f;
            physicsComponent.mDirection.y *= -1.0f;
        }
        else if (transformComponent.mPosition.y - physicsComponent.mCollidableDimensions.y * 0.5f < sceneBottomBound)
        {
            transformComponent.mPosition.y = sceneBottomBound + physicsComponent.mCollidableDimensions.y * 0.5f;
            physicsComponent.mDirection.y *= -1.0f;
        }
        
        physicsComponent.mDirection = glm::normalize(physicsComponent.mDirection);
    }
}

///-----------------------------------------------------------------------------------------------

}
