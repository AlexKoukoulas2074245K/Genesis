///------------------------------------------------------------------------------------------------
///  ConsoleManagementSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 06/01/2020.
///-----------------------------------------------------------------------------------------------

#include "ConsoleManagementSystem.h"
#include "../components/ConsoleStateSingletonComponent.h"
#include "../../rendering/components/RenderableComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------

ConsoleManagementSystem::ConsoleManagementSystem()
    : BaseSystem()
{       
    ecs::World::GetInstance().SetSingletonComponent<ConsoleStateSingletonComponent>(std::make_unique<ConsoleStateSingletonComponent>());    
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::VUpdateAssociatedComponents(const float dt) const
{   
    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<ConsoleStateSingletonComponent>();
    if (consoleStateComponent.mEnabled)
    {
        consoleStateComponent.mBackgroundOpaqueness += dt;
        if (consoleStateComponent.mBackgroundOpaqueness > 0.5f) consoleStateComponent.mBackgroundOpaqueness = 0.5f;
    }
    else
    {
        consoleStateComponent.mBackgroundOpaqueness -= dt;
        if (consoleStateComponent.mBackgroundOpaqueness < 0.0f) consoleStateComponent.mBackgroundOpaqueness = 0.0f;
    }

    const auto consoleBackgroundEntity = ecs::World::GetInstance().FindEntity(StringId("console_background"));
    auto& consoleBackgroundRenderableComponent = ecs::World::GetInstance().GetComponent<rendering::RenderableComponent>(consoleBackgroundEntity);
    consoleBackgroundRenderableComponent.mShaderUniforms.mShaderFloatUniforms[StringId("opaqueness")] = consoleStateComponent.mBackgroundOpaqueness;
}

///-----------------------------------------------------------------------------------------------

}

}