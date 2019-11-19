///------------------------------------------------------------------------------------------------
///  GenesisEngine.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef GenesisEngine_h
#define GenesisEngine_h

///------------------------------------------------------------------------------------------------

#include "IGame.h"
#include "ECS.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

class GenesisEngine final
{
public:
    GenesisEngine();

    void RunGame(IGame& game);

private:
    void Initialize();

private:
    ecs::World mWorld;
};

}

///------------------------------------------------------------------------------------------------

#endif /* GenesisEngine_h */
