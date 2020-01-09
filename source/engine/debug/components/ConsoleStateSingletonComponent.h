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
    ecs::EntityId mCurrentCommandRenderedTextEntityId = ecs::NULL_ENTITY_ID;    
    std::string mCurrentCommandTextBuffer             = "";
    float mBackgroundOpaqueness                       = 0.0f;
    bool mEnabled                                     = false;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ConsoleStateSingletonComponent_h */
