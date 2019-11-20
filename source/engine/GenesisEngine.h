///------------------------------------------------------------------------------------------------
///  GenesisEngine.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef GenesisEngine_h
#define GenesisEngine_h

///------------------------------------------------------------------------------------------------

#include "ECS.h"
#include "GameStartupParameters.h"
#include "IGame.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

class GenesisEngine final
{
public:
    GenesisEngine();

    void RunGame(const GameStartupParameters& startupParameters, IGame& game);

private:
    void Initialize(const GameStartupParameters& startupParameters);
    void InitializeSdlContextAndWindow(const GameStartupParameters& startupParameters);
    void InitializeSystems();
    void InitializeServices() const;

private:
    ecs::World mWorld;
};

}

///------------------------------------------------------------------------------------------------

#endif /* GenesisEngine_h */
