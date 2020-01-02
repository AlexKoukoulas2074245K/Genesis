///------------------------------------------------------------------------------------------------
///  IGame.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef IGame_h
#define IGame_h

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
    virtual void VOnInit() = 0;

    /// Game update method. 
    ///
    /// This will be called once per frame and should be used to 
    /// perform all game-specific logic.
    /// @param[in] dt the inter-frame delta time.    
    virtual void VOnUpdate(const float dt) = 0;
};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* IGame_h */
