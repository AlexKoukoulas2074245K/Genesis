#include "LuaScript.h"

LuaScript::LuaScript(const std::string& filename)
{
    mLuaState = luaL_newstate();
    luaL_openlibs(mLuaState);
    
    const auto errorCode = luaL_loadfile(mLuaState, filename.c_str());
    if (errorCode || lua_pcall(mLuaState, 0, 0, 0))
    {
        std::cout << "Error: script not loaded: " << lua_tostring(mLuaState, -1) << std::endl;
        mLuaState = 0;
    }
}

LuaScript::~LuaScript()
{
    if (mLuaState)
    {
        lua_close(mLuaState);
    }
}

void LuaScript::printError(const std::string& variableName, const std::string& reason)
{
    std::cout << "Error: can't get [" << variableName << "]. " << reason << std::endl;
}

std::vector<int> LuaScript::getIntVector(const std::string& name) {
    std::vector<int> v;
    lua_getglobal(mLuaState, name.c_str());
    if(lua_isnil(mLuaState, -1)) {
        return std::vector<int>();
    }
    lua_pushnil(mLuaState);
    while(lua_next(mLuaState, -2)) {
        v.push_back((int)lua_tonumber(mLuaState, -1));
        lua_pop(mLuaState, 1);
    }
    clean();
    return v;
}

