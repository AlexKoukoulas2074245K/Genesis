///------------------------------------------------------------------------------------------------
///  ConsoleStateSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 06/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef ConsoleStateSingletonComponent_h
#define ConsoleStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

#include <string>
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------

class ConsoleStateSingletonComponent final: public ecs::IComponent
{
public:
    std::vector<std::string> mCommandHistory;
    std::vector<ecs::EntityId> mPastConsoleTextStringEntityIds;
    ecs::EntityId mCurrentCommandRenderedTextEntityId = ecs::NULL_ENTITY_ID;    
    std::string mCurrentCommandTextBuffer             = "";
    float mBackgroundOpaqueness                       = 0.0f;
    int mCommandHistoryIndex                          = -1;
    bool mEnabled                                     = false;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ConsoleStateSingletonComponent_h */
