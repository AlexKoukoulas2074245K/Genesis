///------------------------------------------------------------------------------------------------
///  ShaderStoreSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef ShaderStoreSingletonComponent_h
#define ShaderStoreSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../resources/ShaderResource.h"

#include <memory>
#include <unordered_map>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

class ShaderStoreSingletonComponent final: public ecs::IComponent
{
public:
    std::unordered_map<StringId, res::ShaderResource, StringIdHasher> mShaders;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* ShaderStoreSingletonComponent_h */
