///------------------------------------------------------------------------------------------------
///  GameStartupParameters.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef GameStartupParameters_h
#define GameStartupParameters_h

///------------------------------------------------------------------------------------------------

#include <string>

#include <SDL_video.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------
/// Container for all startup parameters such as title, window size, etc.
struct GameStartupParameters
{
    /// Constructor using exact window dimensions.
    /// @param[in] gameTitle the window title for the game.
    /// @param[in] gameWindowWidth the desired window width in pixels.
    /// @param[in] gameWindowHeight the desired window height in pixels.
    GameStartupParameters
    (
        const std::string& gameTitle,
        const int gameWindowWidth, 
        const int gameWindowHeight
    )
        : mGameTitle(gameTitle)
        , mGameWindowWidth(gameWindowWidth)
        , mGameWindowHeight(gameWindowHeight)
        , mGameWindowScreenFraction(0.0f)
    {
    }

    /// Constructor using screen fraction.
    ///
    /// The exact window dimensions will be calculated based on the fraction passed in, and the aspect 
    /// ratio of the screen.
    /// @param[in] gameTitle the window title for the game
    /// @param[in] gameWindowScreenFraction the desired screen space fraction that the game window will occupy
    GameStartupParameters
    (
        const std::string& gameTitle,
        const float gameWindowScreenFraction
    )
        : mGameTitle(gameTitle)
        , mGameWindowWidth(0)
        , mGameWindowHeight(0)
        , mGameWindowScreenFraction(gameWindowScreenFraction)
    {
        
    }

    const std::string mGameTitle;
    const float mGameWindowScreenFraction;
    const int mGameWindowWidth;
    const int mGameWindowHeight;
};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* GameStartupParameters_h */
