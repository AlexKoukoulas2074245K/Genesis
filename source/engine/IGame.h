///------------------------------------------------------------------------------------------------
///  IGame.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef IGame_h
#define IGame_h

///------------------------------------------------------------------------------------------------

#include "ECS.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------
/// The interface that lays out the core methods for the Game. This should be subclassed 
/// and supplied to the Engine's RunGame method \see GenesisEngine::RunGame()
class IGame
{
public:    
    virtual ~IGame() = default;

    /// Game initialization method. 
    ///
    /// This will be called after all core engine subsystems are initialized, and 
    /// the game is now free to perform all its initialization flows.
    /// @param[in] world the singular world of the ECS state.
    virtual void VOnInit(ecs::World& world) = 0;

    /// Game update method. 
    ///
    /// This will be called once per frame and should be used to 
    /// perform all game-specific logic.
    /// @param[in] dt the inter-frame delta time.
    /// @param[in] world the singular world of the ECS state.
    virtual void VOnUpdate(const float dt, ecs::World& world) = 0;
};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* IGame_h */
