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
    static const glm::vec3 CONSOLE_CURRENT_COMMAND_TEXT_POSITION = glm::vec3(-0.95f, -0.65f, -0.1f);

    static const StringId CONSOLE_TEXT_FONT_NAME          = StringId("console_font");
    static const StringId CONSOLE_BACKGROUND_ENTITY_NAME  = StringId("console_background");
    static const StringId CONSOLE_OPAQUENESS_UNIFORM_NAME = StringId("opaqueness");

    static const float CONSOLE_TEXT_SIZE       = 0.1f;
    static const float CONSOLE_DARKENING_SPEED = 0.005f;
    static const float MAX_OPAQUENESS          = 0.8f;
    
    static const int CONSOLE_MAX_LINES_VISIBLE = 15;

    static const char CONSOLE_PROMPT_CHARACTER = '>';
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
            consoleStateComponent.mCurrentCommandTextBuffer = CONSOLE_PROMPT_CHARACTER;
            SDL_StartTextInput();
        }
        else
        {
            SDL_StopTextInput();
            consoleStateComponent.mCurrentCommandTextBuffer.clear();

            for (const auto& pastConsoleTextStringEntityIds : consoleStateComponent.mPastConsoleTextStringEntityIds)
            {
                rendering::ClearRenderedText(pastConsoleTextStringEntityIds);
            }

            consoleStateComponent.mPastConsoleTextStringEntityIds.clear();
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
    // Handle previous command cycling
    else if (input::IsActionTypeKeyTapped(input::InputActionType::UP_ARROW_KEY))
    {
        if (consoleStateComponent.mCommandHistory.size() > 0)
        {
            consoleStateComponent.mCommandHistoryIndex = math::Min(static_cast<int>(consoleStateComponent.mCommandHistory.size()) - 1, consoleStateComponent.mCommandHistoryIndex + 1);            
            const auto& targetCommandText = consoleStateComponent.mCommandHistory.at(consoleStateComponent.mCommandHistoryIndex);
            consoleStateComponent.mCurrentCommandTextBuffer = CONSOLE_PROMPT_CHARACTER + targetCommandText;
        }
    }
    // Handle next command cycling
    else if (input::IsActionTypeKeyTapped(input::InputActionType::DOWN_ARROW_KEY))
    {
        if (consoleStateComponent.mCommandHistory.size() > 0)
        {
            consoleStateComponent.mCommandHistoryIndex = math::Max(0, consoleStateComponent.mCommandHistoryIndex - 1);
            const auto& targetCommandText = consoleStateComponent.mCommandHistory.at(consoleStateComponent.mCommandHistoryIndex);
            consoleStateComponent.mCurrentCommandTextBuffer = CONSOLE_PROMPT_CHARACTER + targetCommandText;
        }
    }
    // Handle command execution
    else if (input::IsActionTypeKeyTapped(input::InputActionType::ENTER_KEY))
    {
        ExecuteCommand();
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

void ConsoleManagementSystem::ExecuteCommand() const
{
    const auto& world = ecs::World::GetInstance();
    auto& consoleStateComponent = world.GetSingletonComponent<ConsoleStateSingletonComponent>();
    
    consoleStateComponent.mPastConsoleTextStringEntityIds.push_back(consoleStateComponent.mCurrentCommandRenderedTextEntityId);
    
    if (consoleStateComponent.mPastConsoleTextStringEntityIds.size() > CONSOLE_MAX_LINES_VISIBLE)
    {
        rendering::ClearRenderedText(consoleStateComponent.mPastConsoleTextStringEntityIds.front());
        consoleStateComponent.mPastConsoleTextStringEntityIds.erase(consoleStateComponent.mPastConsoleTextStringEntityIds.begin());
    }

    if (consoleStateComponent.mCurrentCommandTextBuffer.size() != 1)
    {
        consoleStateComponent.mCommandHistory.insert(consoleStateComponent.mCommandHistory.begin(), consoleStateComponent.mCurrentCommandTextBuffer.substr(1));
        consoleStateComponent.mCommandHistoryIndex = -1;
    }
    
    consoleStateComponent.mCurrentCommandRenderedTextEntityId = ecs::NULL_ENTITY_ID;
    consoleStateComponent.mCurrentCommandTextBuffer = CONSOLE_PROMPT_CHARACTER;
    
    for (auto i = static_cast<int>(consoleStateComponent.mPastConsoleTextStringEntityIds.size()) - 1; i >= 0; --i)
    {
        const auto positionDisplacementIndex = consoleStateComponent.mPastConsoleTextStringEntityIds.size() - i;
        const auto pastConsoleTextStringEntityId = consoleStateComponent.mPastConsoleTextStringEntityIds.at(static_cast<size_t>(i));
        const auto targetPosition = glm::vec3
        (
            CONSOLE_CURRENT_COMMAND_TEXT_POSITION.x, 
            CONSOLE_CURRENT_COMMAND_TEXT_POSITION.y + positionDisplacementIndex * CONSOLE_TEXT_SIZE, 
            CONSOLE_CURRENT_COMMAND_TEXT_POSITION.z
        );

        rendering::SetTextPosition(pastConsoleTextStringEntityId, targetPosition);
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
