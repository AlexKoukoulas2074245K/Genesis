#ifndef LuaScript_h
#define LuaScript_h

#include <string>
#include <vector>
#include <iostream>

// Lua is written in C, so compiler needs to know how to link its libraries
extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

class LuaScript {
public:
    LuaScript(const std::string& filename);
    ~LuaScript();
    void printError(const std::string& variableName, const std::string& reason);
    std::vector<int> getIntVector(const std::string& name);
    std::vector<std::string> getTableKeys(const std::string& name);
    
    inline void clean() {
        int n = lua_gettop(mLuaState);
        lua_pop(mLuaState, n);
    }
    
    template<typename T>
    T get(const std::string& variableName) {
        if(!mLuaState) {
            printError(variableName, "Script is not loaded");
            return lua_getdefault<T>();
        }
        
        T result;
        if(lua_gettostack(variableName)) { // variable succesfully on top of stack
            result = lua_get<T>(variableName);
        } else {
            result = lua_getdefault<T>();
        }
        
        
        clean();
        return result;
    }
    
    bool lua_gettostack(const std::string& variableName) {
        mLevel = 0;
        std::string var = "";
        for(unsigned int i = 0; i < variableName.size(); i++) {
            if(variableName.at(i) == '.') {
                if(mLevel == 0) {
                    lua_getglobal(mLuaState, var.c_str());
                } else {
                    lua_getfield(mLuaState, -1, var.c_str());
                }
                
                if(lua_isnil(mLuaState, -1)) {
                    printError(variableName, var + " is not defined");
                    return false;
                } else {
                    var = "";
                    mLevel++;
                }
            } else {
                var += variableName.at(i);
            }
        }
        if(mLevel == 0) {
            lua_getglobal(mLuaState, var.c_str());
        } else {
            lua_getfield(mLuaState, -1, var.c_str());
        }
        if(lua_isnil(mLuaState, -1)) {
            printError(variableName, var + " is not defined");
            return false;
        }
        
        return true;
    }
    
    // Generic get
    template<typename T>
    T lua_get(const std::string&) {
        return 0;
    }
    
    template<typename T>
    T lua_getdefault() {
        return 0;
    }
    
    int callFuncGetVal()
    {
        lua_getglobal(mLuaState, "getVal");
        
        std::cout<<"loaded"<<std::endl;
        lua_pcall(mLuaState, 0, 1, 0);
        
        int result = (int)lua_tonumber(mLuaState, -1);
        lua_pop(mLuaState, 1);
        return result;
    }
    
    lua_State* mLuaState;
    
private:
    int mLevel;
};

// Specializations

template <>
inline bool LuaScript::lua_get<bool>(const std::string&) {
    return (bool)lua_toboolean(mLuaState, -1);
}

template <>
inline float LuaScript::lua_get<float>(const std::string& variableName) {
    if(!lua_isnumber(mLuaState, -1)) {
        printError(variableName, "Not a number");
    }
    return (float)lua_tonumber(mLuaState, -1);
}

template <>
inline int LuaScript::lua_get<int>(const std::string& variableName) {
    if(!lua_isnumber(mLuaState, -1)) {
        printError(variableName, "Not a number");
    }
    return (int)lua_tonumber(mLuaState, -1);
}

template <>
inline std::string LuaScript::lua_get<std::string>(const std::string& variableName) {
    std::string s = "null";
    if(lua_isstring(mLuaState, -1)) {
        s = std::string(lua_tostring(mLuaState, -1));
    } else {
        printError(variableName, "Not a string");
    }
    return s;
}

template<>
inline std::string LuaScript::lua_getdefault<std::string>() {
    return "null";
}

#endif /* LuaScript_h */
