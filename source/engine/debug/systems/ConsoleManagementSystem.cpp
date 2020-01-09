///------------------------------------------------------------------------------------------------
///  ConsoleManagementSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 06/01/2020.
///-----------------------------------------------------------------------------------------------

#include "ConsoleManagementSystem.h"
#include "../components/ConsoleStateSingletonComponent.h"
#include "../../input/utils/InputUtils.h"
#include "../../rendering/components/RenderableComponent.h"
#include "../../rendering/components/TextStringComponent.h"
#include "../../rendering/utils/FontUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const glm::vec4 CONSOLE_TEXT_COLOR                    = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    static const glm::vec3 CONSOLE_CURRENT_COMMAND_TEXT_POSITION = glm::vec3(-0.95f, 0.65f, -0.1f);

    static const StringId CONSOLE_TEXT_FONT_NAME          = StringId("console_font");
    static const StringId CONSOLE_BACKGROUND_ENTITY_NAME  = StringId("console_background");
    static const StringId CONSOLE_OPAQUENESS_UNIFORM_NAME = StringId("opaqueness");

    static const float CONSOLE_TEXT_SIZE       = 0.1f;
    static const float CONSOLE_DARKENING_SPEED = 0.005f;
    static const float MAX_OPAQUENESS          = 0.8f;    
}

///-----------------------------------------------------------------------------------------------

ConsoleManagementSystem::ConsoleManagementSystem()
    : BaseSystem()
{       
    ecs::World::GetInstance().SetSingletonComponent<ConsoleStateSingletonComponent>(std::make_unique<ConsoleStateSingletonComponent>());    
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::VUpdateAssociatedComponents(const float) const
{   
    HandleConsoleSpecialInput();
    HandleConsoleBackgroundAnimation();
    HandleConsoleTextRendering();
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::HandleConsoleSpecialInput() const
{
    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<debug::ConsoleStateSingletonComponent>();

    // Handling console open/close
    if (input::IsActionTypeKeyTapped(input::InputActionType::CONSOLE_TOGGLE))
    {
        consoleStateComponent.mEnabled = !consoleStateComponent.mEnabled;

        if (consoleStateComponent.mEnabled)
        {   
            consoleStateComponent.mCurrentCommandTextBuffer = ">";
            SDL_StartTextInput();
        }
        else
        {
            SDL_StopTextInput();
            consoleStateComponent.mCurrentCommandTextBuffer.clear();
        }
    }
    // Handle console backspace
    else if (input::IsActionTypeKeyTapped(input::InputActionType::BACKSPACE_KEY))
    {
        if (consoleStateComponent.mEnabled && consoleStateComponent.mCurrentCommandTextBuffer.size() > 1)
        {
            auto& consoleCurrentCommandText = consoleStateComponent.mCurrentCommandTextBuffer;
            consoleCurrentCommandText       = consoleCurrentCommandText.substr(0, consoleCurrentCommandText.size() - 1);            
        }
    }
}

///-----------------------------------------------------------------------------------------------

void ConsoleManagementSystem::HandleConsoleBackgroundAnimation() const
{
    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<ConsoleStateSingletonComponent>();
    if (consoleStateComponent.mEnabled)
    {
        // dt is not used in the opaqueness calculation since frozen when console opens
        consoleStateComponent.mBackgroundOpaqueness += CONSOLE_DARKENING_SPEED;
        if (consoleStateComponent.mBackgroundOpaqueness > MAX_OPAQUENESS)
        {
            consoleStateComponent.mBackgroundOpaqueness = MAX_OPAQUENESS;
        }
    }
    else
    {
        // dt is not used in the opaqueness calculation since frozen when console opens
        consoleStateComponent.mBackgroundOpaqueness -= CONSOLE_DARKENING_SPEED;
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

void ConsoleManagementSystem::HandleConsoleTextRendering() const
{
    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<ConsoleStateSingletonComponent>();

    if (consoleStateComponent.mCurrentCommandRenderedTextEntityId == ecs::NULL_ENTITY_ID || IsCurrentCommandRenderedTextOutOfDate())
    {
        if (consoleStateComponent.mCurrentCommandRenderedTextEntityId != ecs::NULL_ENTITY_ID)
        {
            rendering::ClearRenderedText(consoleStateComponent.mCurrentCommandRenderedTextEntityId);            
        }

        consoleStateComponent.mCurrentCommandRenderedTextEntityId = rendering::RenderText
        (
            consoleStateComponent.mCurrentCommandTextBuffer,
            CONSOLE_TEXT_FONT_NAME,
            CONSOLE_TEXT_SIZE,
            CONSOLE_CURRENT_COMMAND_TEXT_POSITION,
            CONSOLE_TEXT_COLOR
        );        
    }    
}

///-----------------------------------------------------------------------------------------------

bool ConsoleManagementSystem::IsCurrentCommandRenderedTextOutOfDate() const
{
    const auto& world = ecs::World::GetInstance();
    const auto& consoleStateComponent = world.GetSingletonComponent<ConsoleStateSingletonComponent>();
    const auto& renderedTextStringComponent = world.GetComponent<rendering::TextStringComponent>(consoleStateComponent.mCurrentCommandRenderedTextEntityId);

    std::string renderedCommandText = "";
    for (const auto& characterEntry: renderedTextStringComponent.mTextCharacterEntities)
    {
        renderedCommandText += characterEntry.mCharacter;
    }

    return renderedCommandText != consoleStateComponent.mCurrentCommandTextBuffer;
}

///-----------------------------------------------------------------------------------------------

}

}