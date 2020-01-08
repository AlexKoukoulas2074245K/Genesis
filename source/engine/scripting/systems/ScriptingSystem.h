///------------------------------------------------------------------------------------------------
///  ScriptingSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 08/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef ScriptingSystem_h
#define ScriptingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace scripting
{

///-----------------------------------------------------------------------------------------------

class ScriptingSystem final : public ecs::BaseSystem
{
public:
    ScriptingSystem();

    void VUpdateAssociatedComponents(const float dt) const override;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ScriptingSystem_h */