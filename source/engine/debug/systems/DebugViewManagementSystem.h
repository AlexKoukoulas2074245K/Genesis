///------------------------------------------------------------------------------------------------
///  DebugViewManagementSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 13/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef DebugViewManagementSystem_h
#define DebugViewManagementSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------
class DebugViewManagementSystem final : public ecs::BaseSystem
{
public:
    DebugViewManagementSystem();

    void VUpdateAssociatedComponents(const float dt) const override;

private:
    void HandleFrameStatsDisplay() const;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* DebugViewManagementSystem_h */
