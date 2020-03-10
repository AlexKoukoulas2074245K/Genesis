///------------------------------------------------------------------------------------------------
///  DefaultEngineConsoleCommands.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 03/10/2019.
///------------------------------------------------------------------------------------------------

#include "DefaultEngineConsoleCommands.h"
#include "components/DebugViewStateSingletonComponent.h"
#include "utils/ConsoleCommandUtils.h"
#include "../common/components/TransformComponent.h"

#include <unordered_set>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace debug
{

///------------------------------------------------------------------------------------------------

void RegisterDefaultEngineConsoleCommands()
{
#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)    
    debug::RegisterConsoleCommand(StringId("commands"), [](const std::vector<std::string>& commandTextComponents)
    {
        const std::string USAGE_STRING = "Usage: commands";

        if (commandTextComponents.size() != 1)
        {
            return debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        const auto& world = ecs::World::GetInstance();
        auto& consoleStateComponent = world.GetSingletonComponent<debug::ConsoleStateSingletonComponent>();

        // Gather command names
        std::vector<std::string> commandNames;
        for (const auto& registeredConsoleCommandEntry : consoleStateComponent.mRegisterdConsoleCommands)
        {
            commandNames.push_back(registeredConsoleCommandEntry.first.GetString());
        }

        // Sort command names
        std::sort(commandNames.begin(), commandNames.end());

        // Print command names alphabetically line by line        
        std::string output = "Available commands:\n";

        for (const auto& commandName : commandNames)
        {
            output += commandName + '\n';
        }

        return debug::ConsoleCommandResult(true, output);
    });

    debug::RegisterConsoleCommand(StringId("frame_stats"), [](const std::vector<std::string>& commandTextComponents)
    {
        static const std::unordered_set<std::string> sAllowedOptions = { "on", "off" };

        const std::string USAGE_STRING = "Usage: frame_stats on|off";

        if (commandTextComponents.size() != 2 || sAllowedOptions.count(StringToLower(commandTextComponents[1])) == 0)
        {
            return debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        const auto& world = ecs::World::GetInstance();
        auto& debugViewStateComponent = world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>();

        debugViewStateComponent.mFrameStatsDisplayEnabled = StringToLower(commandTextComponents[1]) == "on";

        return debug::ConsoleCommandResult(true);
    });

    debug::RegisterConsoleCommand(StringId("move_entity"), [](const std::vector<std::string>& commandTextComponents)
    {
        const std::string USAGE_STRING = "Usage: move_entity \"entity_name\" dx dy dz";
        const std::string ENTITY_NOT_FOUND_STRING = "Entity with given name not found!";
        const std::string ENTITY_NO_TRANSFORM_COMPONENT_STRING = "Entity does not have a TransformComponent!";

        if (commandTextComponents.size() != 5 || !StringStartsWith(commandTextComponents[1], "\""))
        {
            return debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        // extract entity name from quotes and position deltas
        const auto entityName = StringSplit(commandTextComponents[1], '\"')[1];
        const auto dx = std::stof(commandTextComponents[2]);
        const auto dy = std::stof(commandTextComponents[3]);
        const auto dz = std::stof(commandTextComponents[4]);

        const auto& world = ecs::World::GetInstance();

        // make sure entity exists
        const auto foundEntityId = world.FindEntity(entityName);
        if (foundEntityId == ecs::NULL_ENTITY_ID)
        {
            return debug::ConsoleCommandResult(false, ENTITY_NOT_FOUND_STRING);
        }

        // make sure entity has transform component
        if (!world.HasComponent<TransformComponent>(foundEntityId))
        {
            return debug::ConsoleCommandResult(false, ENTITY_NO_TRANSFORM_COMPONENT_STRING);
        }

        auto& transformComponent = world.GetComponent<TransformComponent>(foundEntityId);
        transformComponent.mPosition.x += dx;
        transformComponent.mPosition.y += dy;
        transformComponent.mPosition.z += dz;

        return debug::ConsoleCommandResult(true);
    });

    debug::RegisterConsoleCommand(StringId("set_entity_position"), [](const std::vector<std::string>& commandTextComponents)
    {
        const std::string USAGE_STRING = "Usage: set_entity_position \"entity_name\" x y z";
        const std::string ENTITY_NOT_FOUND_STRING = "Entity with given name not found!";
        const std::string ENTITY_NO_TRANSFORM_COMPONENT_STRING = "Entity does not have a TransformComponent!";

        if (commandTextComponents.size() != 5 || !StringStartsWith(commandTextComponents[1], "\""))
        {
            return debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        // extract entity name from quotes and position deltas
        const auto entityName = StringSplit(commandTextComponents[1], '\"')[1];
        const auto posX = std::stof(commandTextComponents[2]);
        const auto posY = std::stof(commandTextComponents[3]);
        const auto posZ = std::stof(commandTextComponents[4]);

        const auto& world = ecs::World::GetInstance();

        // make sure entity exists
        const auto foundEntityId = world.FindEntity(entityName);
        if (foundEntityId == ecs::NULL_ENTITY_ID)
        {
            return debug::ConsoleCommandResult(false, ENTITY_NOT_FOUND_STRING);
        }

        // make sure entity has transform component
        if (!world.HasComponent<TransformComponent>(foundEntityId))
        {
            return debug::ConsoleCommandResult(false, ENTITY_NO_TRANSFORM_COMPONENT_STRING);
        }

        auto& transformComponent = world.GetComponent<TransformComponent>(foundEntityId);
        transformComponent.mPosition.x = posX;
        transformComponent.mPosition.y = posY;
        transformComponent.mPosition.z = posZ;

        return debug::ConsoleCommandResult(true);
    });

    debug::RegisterConsoleCommand(StringId("get_entity_position"), [](const std::vector<std::string>& commandTextComponents)
    {
        const std::string USAGE_STRING = "Usage: get_entity_position \"entity_name\"";
        const std::string ENTITY_NOT_FOUND_STRING = "Entity with given name not found!";
        const std::string ENTITY_NO_TRANSFORM_COMPONENT_STRING = "Entity does not have a TransformComponent!";

        if (commandTextComponents.size() != 2 || !StringStartsWith(commandTextComponents[1], "\""))
        {
            return debug::ConsoleCommandResult(false, USAGE_STRING);
        }

        // extract entity name from quotes and position deltas
        const auto entityName = StringSplit(commandTextComponents[1], '\"')[1];

        const auto& world = ecs::World::GetInstance();

        // make sure entity exists
        const auto foundEntityId = world.FindEntity(entityName);
        if (foundEntityId == ecs::NULL_ENTITY_ID)
        {
            return debug::ConsoleCommandResult(false, ENTITY_NOT_FOUND_STRING);
        }

        // make sure entity has transform component
        if (!world.HasComponent<TransformComponent>(foundEntityId))
        {
            return debug::ConsoleCommandResult(false, ENTITY_NO_TRANSFORM_COMPONENT_STRING);
        }

        auto& transformComponent = world.GetComponent<TransformComponent>(foundEntityId);

        return debug::ConsoleCommandResult
        (
            true,
            "Entity position: " +
            std::to_string(transformComponent.mPosition.x) + ", " +
            std::to_string(transformComponent.mPosition.y) + ", " +
            std::to_string(transformComponent.mPosition.z)
        );
    });

#endif
}

///------------------------------------------------------------------------------------------------

}
}

///------------------------------------------------------------------------------------------------
