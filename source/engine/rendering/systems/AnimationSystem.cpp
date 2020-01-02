///------------------------------------------------------------------------------------------------
///  AnimationSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#include "AnimationSystem.h"
#include "../components/AnimationTimerComponent.h"
#include "../components/RenderableComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

AnimationSystem::AnimationSystem()
    : BaseSystem()
{
    CalculateAndSetComponentUsageMask<AnimationTimerComponent, RenderableComponent>();
}

///-----------------------------------------------------------------------------------------------

void AnimationSystem::VUpdateAssociatedComponents(const float dt) const
{
    for (const auto& entityId : ecs::World::GetInstance().GetActiveEntities())
    {
        if (ShouldProcessEntity(entityId))
        {
            auto& animationComponent = ecs::World::GetInstance().GetComponent<AnimationTimerComponent>(entityId);

            animationComponent.mAnimationTimer->Update(dt);

            if (animationComponent.mAnimationTimer->HasTicked())
            {
                animationComponent.mAnimationTimer->Reset();
                
                auto& renderableComponent = ecs::World::GetInstance().GetComponent<RenderableComponent>(entityId);
                const auto& activeAnimationMeshes = renderableComponent.mAnimationsToMeshes.at(renderableComponent.mActiveAnimationNameId);

                renderableComponent.mActiveMeshIndex = (renderableComponent.mActiveMeshIndex + 1) % activeAnimationMeshes.size();
            }                        
        }
    }
}

///-----------------------------------------------------------------------------------------------

}

}