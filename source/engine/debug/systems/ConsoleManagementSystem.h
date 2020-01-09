///------------------------------------------------------------------------------------------------
///  ConsoleManagementSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef ConsoleManagementSystem_h
#define ConsoleManagementSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace debug
{

///-----------------------------------------------------------------------------------------------
class ConsoleManagementSystem final : public ecs::BaseSystem
{
public:
    ConsoleManagementSystem();

    void VUpdateAssociatedComponents(const float dt) const override;
    void HandleConsoleSpecialInput() const;
    void HandleConsoleBackgroundAnimation() const;
    void HandleConsoleTextRendering() const;

    bool IsCurrentCommandRenderedTextOutOfDate() const;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ConsoleManagementSystem_h */
