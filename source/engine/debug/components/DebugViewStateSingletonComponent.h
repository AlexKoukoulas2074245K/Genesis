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

#include <vector>

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
    std::vector<std::pair<ecs::EntityId, ecs::EntityId>> mSystemNamesAndUpdateTimeStrings;    
    std::pair<ecs::EntityId, ecs::EntityId> mFpsStrings;    
    std::pair<ecs::EntityId, ecs::EntityId> mEntityCountStrings;

    int mCurrentFps                  = 0;
    bool mFrameStatsDisplayEnabled   = false;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* DebugViewStateSingletonComponent_h */
