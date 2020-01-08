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

namespace
{
    static const StringId CONSOLE_BACKGROUND_ENTITY_NAME  = StringId("console_background");
    static const StringId CONSOLE_OPAQUENESS_UNIFORM_NAME = StringId("opaqueness");

    static const float CONSOLE_DARKENING_SPEED = 2.0f;
    static const float MAX_OPAQUENESS          = 0.5f;
}

///-----------------------------------------------------------------------------------------------

ConsoleManagementSystem::ConsoleManagementSystem()
    : BaseSystem()
{       
    ecs::World::GetInstance().SetSingletonComponent<ConsoleStateSingletonComponent>(std::make_unique<ConsoleStateSingletonComponent>());    
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::VUpdateAssociatedComponents(const float dt) const
{   
    HandleConsoleBackgroundAnimation(dt);
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::HandleConsoleBackgroundAnimation(const float dt) const
{
    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<ConsoleStateSingletonComponent>();
    if (consoleStateComponent.mEnabled)
    {
        consoleStateComponent.mBackgroundOpaqueness += CONSOLE_DARKENING_SPEED * dt;
        if (consoleStateComponent.mBackgroundOpaqueness > MAX_OPAQUENESS)
        {
            consoleStateComponent.mBackgroundOpaqueness = MAX_OPAQUENESS;
        }
    }
    else
    {
        consoleStateComponent.mBackgroundOpaqueness -= CONSOLE_DARKENING_SPEED * dt;
        if (consoleStateComponent.mBackgroundOpaqueness < 0.0f)
        {
            consoleStateComponent.mBackgroundOpaqueness = 0.0f;
        }
    }

    const auto consoleBackgroundEntity = ecs::World::GetInstance().FindEntity(CONSOLE_BACKGROUND_ENTITY_NAME);
    auto& consoleBackgroundRenderableComponent = ecs::World::GetInstance().GetComponent<rendering::RenderableComponent>(consoleBackgroundEntity);
    consoleBackgroundRenderableComponent.mShaderUniforms.mShaderFloatUniforms[CONSOLE_OPAQUENESS_UNIFORM_NAME] = consoleStateComponent.mBackgroundOpaqueness;
}

///-----------------------------------------------------------------------------------------------

}

}