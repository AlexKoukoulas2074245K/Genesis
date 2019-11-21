///------------------------------------------------------------------------------------------------
///  Main.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "../engine/GenesisEngine.h"

///------------------------------------------------------------------------------------------------

int main(int, char**)
{
    genesis::GenesisEngine engine;
    genesis::GameStartupParameters startupParameters("Genesis", 0.7f);

    Game game;
    engine.RunGame(startupParameters, game);
}

///------------------------------------------------------------------------------------------------


