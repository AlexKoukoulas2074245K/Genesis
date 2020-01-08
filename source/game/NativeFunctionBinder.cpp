///------------------------------------------------------------------------------------------------
///  NativeFunctionBinder.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 06/01/2020.
///------------------------------------------------------------------------------------------------

#include "NativeFunctionBinder.h"
#include "../engine/common/components/TransformComponent.h"
#include "../engine/scripting/service/LuaScriptingService.h"

///------------------------------------------------------------------------------------------------

void NativeFunctionBinder::BindNativeFunctionsToLua()
{
    using genesis::scripting::LuaScriptingService;

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("CreateEntity", "CreateEntity([optional]entityName) -> entityId", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 0)
        {
            luaScriptingService.LuaPushIntegral(genesis::ecs::World::GetInstance().CreateEntity());
        }
        else if (stackSize == 1)
        {
            const auto entityName = StringId(luaScriptingService.LuaToString(1));
            luaScriptingService.LuaPushIntegral(genesis::ecs::World::GetInstance().CreateEntity(entityName));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 0 or 1) when calling CreateEntity");
        }

        return 1;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("FindEntity", "FindEntity(entityName) -> entityId", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityName = StringId(luaScriptingService.LuaToString(1));
            luaScriptingService.LuaPushIntegral(genesis::ecs::World::GetInstance().FindEntity(entityName));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling FindEntity");
        }

        return 1;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("DestroyEntity", "DestroyEntity(entityId) -> void", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            genesis::ecs::World::GetInstance().DestroyEntity(entityId);
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling DestroyEntity");
        }

        return 0;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("GetEntityPosition", "GetEntityPosition(entityId) -> x,y,z", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            const auto& transformComponent = genesis::ecs::World::GetInstance().GetComponent<genesis::TransformComponent>(entityId);
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.x));
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.y));
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.z));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling GetEntityPosition");
        }

        return 3;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("SetEntityPosition", "SetEntityPosition(entityId, x, y, z) -> void", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 4)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            const auto positionX = luaScriptingService.LuaToDouble(2);
            const auto positionY = luaScriptingService.LuaToDouble(3);
            const auto positionZ = luaScriptingService.LuaToDouble(4);

            auto& transformComponent = genesis::ecs::World::GetInstance().GetComponent<genesis::TransformComponent>(entityId);
            transformComponent.mPosition = glm::vec3(positionX, positionY, positionZ);
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 4) when calling SetEntityPosition");
        }

        return 0;
    });

}

///------------------------------------------------------------------------------------------------