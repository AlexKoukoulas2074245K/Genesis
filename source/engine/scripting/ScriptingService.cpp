///------------------------------------------------------------------------------------------------
///  ScriptingService.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 02/01/2020.
///------------------------------------------------------------------------------------------------

#include "ScriptingService.h"
#include "../common/utils/OSMessageBox.h"
#include "../resources/DataFileResource.h"
#include "../resources/ResourceLoadingService.h"

extern "C" 
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------ 

namespace scripting
{

///------------------------------------------------------------------------------------------------ 

namespace
{
    static const std::string SCRIPT_FILE_EXTENSION = ".lua";
}

///------------------------------------------------------------------------------------------------ 

ScriptingService& ScriptingService::GetInstance()
{
    static ScriptingService instance;
    return instance;
}

///------------------------------------------------------------------------------------------------

ScriptingService::~ScriptingService()
{
    lua_close(mLuaState);
    mLuaState = nullptr;
}

///------------------------------------------------------------------------------------------------

void ScriptingService::RunScript(const std::string& scriptName) const
{
    // Get actual script path
    const auto& scriptPath = resources::ResourceLoadingService::RES_SCRIPTS_ROOT + scriptName + SCRIPT_FILE_EXTENSION;

    // Run script
    lua_settop(mLuaState, 0); //empty the lua stack
    if (luaL_dofile(mLuaState, scriptPath.c_str())) 
    {       
        const auto& errorMessage = std::string(lua_tostring(mLuaState, -1));
        ShowMessageBox(MessageBoxType::ERROR, "Script loading error", "Failed to load script:\n" + errorMessage);        
        lua_pop(mLuaState, 1);    
    }    
}

///------------------------------------------------------------------------------------------------

void ScriptingService::BindNativeFunctionToLua(const std::string& functionName, lua_CFunction function) const
{    
    lua_pushcfunction(mLuaState, function);
    lua_setglobal(mLuaState, functionName.c_str());
}

///------------------------------------------------------------------------------------------------

const char* ScriptingService::LuaToString(const int stackIndex) const
{
    return lua_tostring(mLuaState, stackIndex);
}

///------------------------------------------------------------------------------------------------

void ScriptingService::LuaPushInteger(const long long value)
{
    lua_pushinteger(mLuaState, value);
}

///------------------------------------------------------------------------------------------------

void ScriptingService::LuaPushBoolean(const bool value)
{
    lua_pushboolean(mLuaState, value);
}

///------------------------------------------------------------------------------------------------

void ScriptingService::LuaPushString(const std::string& value)
{
    lua_pushstring(mLuaState, value.c_str());
}

///------------------------------------------------------------------------------------------------

/*
void ScriptingService::LuaCallGlobalFunction(const std::string& functionName) const
{
    lua_getglobal(mLuaState, functionName.c_str());
    lua_pushnumber(mLuaState, 0.1f);
    lua_pcall(mLuaState, 1, 0, 0);
}
*/

///------------------------------------------------------------------------------------------------

void ScriptingService::Initialize()
{
    mLuaState = luaL_newstate();
    luaL_openlibs(mLuaState);
}

///------------------------------------------------------------------------------------------------

}

}