///------------------------------------------------------------------------------------------------
///  Main.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "../engine/GenesisEngine.h"

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


///------------------------------------------------------------------------------------------------

int main(int, char**)
{
    lua_State *L = luaL_newstate(); /* opens Lua */
    luaL_openlibs(L);
    
    genesis::GenesisEngine engine;
    genesis::GameStartupParameters startupParameters("Genesis", 0.3f);

    Game game;
    engine.RunGame(startupParameters, game);
}

///------------------------------------------------------------------------------------------------


