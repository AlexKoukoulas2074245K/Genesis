///------------------------------------------------------------------------------------------------
///  ScriptingService.h
///  Genesis
///
///  Created by Alex Koukoulas on 02/01/2020.
///------------------------------------------------------------------------------------------------

#ifndef ScriptingService_h
#define ScriptingService_h

///------------------------------------------------------------------------------------------------

#include "../common/utils/StringUtils.h"
#include "../../engine/GenesisEngine.h"

#include <iostream>
#include <initializer_list>

///------------------------------------------------------------------------------------------------

struct lua_State;

///------------------------------------------------------------------------------------------------

typedef int(*lua_CFunction) (lua_State *L);

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace scripting
{

///------------------------------------------------------------------------------------------------
/// A class providing access to scripting opening/executing utilities.
class ScriptingService final
{        
    friend class genesis::GenesisEngine;

public:   
    /// The default method of getting a hold of this singleton.
    ///
    /// The single instance of this class will be lazily initialized
    /// the first time it is needed.
    /// @returns a reference to the single instance of this class.    
    static ScriptingService& GetInstance();

    ~ScriptingService();
    ScriptingService(const ScriptingService&) = delete;
    ScriptingService(ScriptingService&&) = delete;
    const ScriptingService& operator = (const ScriptingService&) = delete;
    ScriptingService& operator = (ScriptingService&&) = delete;    

    /// Runs a script with the given name.
    ///
    /// @param[in] scriptName the name of the script to run (this is the filename of the music track without the extension).    
    void RunScript(const std::string& scriptName) const;

    /// Binds a native function to the lua state.
    ///
    /// @param[in] functionName the name that will be registered for the function on the lua state.   
    /// @param[in] function the function to be registered on the lua state.   
    void BindNativeFunctionToLua(const std::string& functionName, lua_CFunction function) const; 

    /// Converts the Lua value at the given stack index to a string
    ///
    /// @param[in] stackIndex the stack index to get the value from
    /// @returns an immutable string representation of the value at the given stack index
    const char* LuaToString(const int stackIndex) const;

    /// Pushes an integer value to the Lua stack
    ///
    /// @param[in] value to be pushed to the Lua stack
    void LuaPushInteger(const long long value);

    /// Pushes a boolean value to the Lua stack
    ///
    /// @param[in] value to be pushed to the Lua stack
    void LuaPushBoolean(const bool value);

    /// Pushes a string value to the Lua stack
    ///
    /// @param[in] value to be pushed to the Lua stack
    void LuaPushString(const std::string& value);

   
    void LuaCallGlobalFunctionWithoutArguments(const std::string& functionName);

    enum class LuaArgumentType
    {
        NIL, DOUBLE, INTEGER, STRING
    };

    template<class T>
    struct 
    void LuaCallGlobalFunctionWithArguments(const std::string& functionName, std::initializer_list)

private:    
    ScriptingService() = default;
    
    void Initialize();

    lua_State* mLuaState;
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* ScriptingService_h */
