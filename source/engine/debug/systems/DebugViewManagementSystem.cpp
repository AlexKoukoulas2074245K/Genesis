///------------------------------------------------------------------------------------------------
///  DebugViewManagementSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 13/01/2020.
///-----------------------------------------------------------------------------------------------

#include "DebugViewManagementSystem.h"
#include "../components/DebugViewStateSingletonComponent.h"
#include "../../rendering/utils/FontUtils.h"
#include "../../rendering/utils/Colors.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------

namespace
{    
    static const glm::vec3 FPS_TEXT_POSITION = glm::vec3(0.65f, 0.0f, 0.0f);
    static const StringId TEXT_FONT_NAME     = StringId("console_font");    
    static const float TEXT_SIZE             = 0.1f;    
}

///-----------------------------------------------------------------------------------------------

DebugViewManagementSystem::DebugViewManagementSystem()
    : BaseSystem()
{       
    ecs::World::GetInstance().SetSingletonComponent<DebugViewStateSingletonComponent>(std::make_unique<DebugViewStateSingletonComponent>());        
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::VUpdateAssociatedComponents(const float) const
{
    HandleFrameStatsDisplay();
}

///-----------------------------------------------------------------------------------------------

void DebugViewManagementSystem::HandleFrameStatsDisplay() const
{
    const auto& world = ecs::World::GetInstance();
    auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();

    if (debugViewStateComponent.mFrameStatsDisplayEnabled)
    {
        if (debugViewStateComponent.mFpsStringEntityId != ecs::NULL_ENTITY_ID)
        {
            rendering::DestroyRenderedText(debugViewStateComponent.mFpsStringEntityId);
            debugViewStateComponent.mFpsStringEntityId = ecs::NULL_ENTITY_ID;
        }

        auto fpsTextColor = rendering::colors::BLACK;
        if (debugViewStateComponent.mCurrentFps < 50)
        {
            fpsTextColor = rendering::colors::YELLOW;
        }
        if (debugViewStateComponent.mCurrentFps < 20)
        {
            fpsTextColor = rendering::colors::RED;
        }

        debugViewStateComponent.mFpsStringEntityId = rendering::RenderText
        (
            "FPS: " + std::to_string(debugViewStateComponent.mCurrentFps),
            TEXT_FONT_NAME,
            TEXT_SIZE,
            FPS_TEXT_POSITION,
            fpsTextColor
        );
    }
    else
    {
        if (debugViewStateComponent.mFpsStringEntityId != ecs::NULL_ENTITY_ID)
        {
            rendering::DestroyRenderedText(debugViewStateComponent.mFpsStringEntityId);
            debugViewStateComponent.mFpsStringEntityId = ecs::NULL_ENTITY_ID;
        }
    }
}

///-----------------------------------------------------------------------------------------------

}

}
