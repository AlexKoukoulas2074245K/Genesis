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

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

struct GameStartupParameters
{
    GameStartupParameters
    (
        const std::string& gameTitle,
        const int gameWindowWidth, 
        const int gameWindowHeight
    )
        : mGameTitle(gameTitle)
        , mGameWindowWidth(gameWindowWidth)
        , mGameWindowHeight(gameWindowHeight)
    {
    }

    const std::string mGameTitle;
    const int mGameWindowWidth;
    const int mGameWindowHeight;
};

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* GameStartupParameters_h */
