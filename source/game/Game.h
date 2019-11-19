///------------------------------------------------------------------------------------------------
///  Game.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef App_h
#define App_h

///------------------------------------------------------------------------------------------------

#include "../engine/IGame.h"

///------------------------------------------------------------------------------------------------

class Game final: public IGame
{
public:    
    void VOnInit(genesis::ecs::World& world) override;
    void VOnUpdate(const float dt, genesis::ecs::World& world) override;

};       

///------------------------------------------------------------------------------------------------

#endif /* Game_h */
