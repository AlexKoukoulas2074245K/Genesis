///------------------------------------------------------------------------------------------------
///  ScriptingSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 08/01/2020.
///-----------------------------------------------------------------------------------------------

#include "ScriptingSystem.h"
#include "../components/ScriptComponent.h"
#include "../service/LuaScriptingService.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace scripting
{

///-----------------------------------------------------------------------------------------------
  
static std::unordered_map<ScriptType, std::string> sScriptTypeToLuaFuncName = 
{
    { ScriptType::CONTINUOUS_EXECUTION, "Update" },
    { ScriptType::SINGLE_EXECUTION, "Execute" }
};

///-----------------------------------------------------------------------------------------------

ScriptingSystem::ScriptingSystem()
    : BaseSystem()
{
    CalculateAndSetComponentUsageMask<ScriptComponent>();
}

///-----------------------------------------------------------------------------------------------

void ScriptingSystem::VUpdateAssociatedComponents(const float dt) const
{
    static float dtAccum = 0.0f;
    dtAccum += dt;

    for (const auto& entityId : ecs::World::GetInstance().GetActiveEntities())
    {
        if (ShouldProcessEntity(entityId))
        {
            const auto& scriptComponent = ecs::World::GetInstance().GetComponent<ScriptComponent>(entityId);
            LuaScriptingService::GetInstance().RunLuaScript(scriptComponent.mScriptName.GetString());            
            LuaScriptingService::GetInstance().LuaCallGlobalFunction(sScriptTypeToLuaFuncName.at(scriptComponent.mScriptType), 3, entityId, dt, dtAccum);            
        }
    }
}

///-----------------------------------------------------------------------------------------------

}

}