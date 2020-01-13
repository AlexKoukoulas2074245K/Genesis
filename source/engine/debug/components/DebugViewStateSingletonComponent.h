///------------------------------------------------------------------------------------------------
///  DebugViewStateSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 13/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef DebugViewStateSingletonComponent_h
#define DebugViewStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{


///-----------------------------------------------------------------------------------------------

class DebugViewStateSingletonComponent final: public ecs::IComponent
{
public:
    ecs::EntityId mFpsStringEntityId = ecs::NULL_ENTITY_ID;
    int mCurrentFps                  = 0;
    bool mFrameStatsDisplayEnabled   = false;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* DebugViewStateSingletonComponent_h */
