///------------------------------------------------------------------------------------------------
///  FontsStoreSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef FontsStoreSingletonComponent_h
#define FontsStoreSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../resources/ResourceLoadingService.h"

#include <unordered_map>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

class FontsStoreSingletonComponent final: public ecs::IComponent
{
public:
    std::unordered_map<StringId, std::unordered_map<char, resources::ResourceId>, StringIdHasher> mLoadedFonts;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* FontsStoreSingletonComponent_h */
