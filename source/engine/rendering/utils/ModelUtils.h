///------------------------------------------------------------------------------------------------
///  ModelUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef ModelUtils_h
#define ModelUtils_h

///------------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/StringUtils.h"
#include "../../resources/ResourceLoadingService.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------

class RenderableComponent;

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateModelByName
(
    const std::string& modelName,
    const glm::vec3& initialPosition,
    ecs::World& world
);

///------------------------------------------------------------------------------------------------

resources::ResourceId LoadMeshResourceFromAtlasTexCoords
(
    const int meshAtlasCol,
    const int meshAtlasRow,
    const int atlasCols,
    const int atlasRows,
    const bool horFlipped,
    const std::string& modelName
);

///------------------------------------------------------------------------------------------------

resources::ResourceId LoadMeshFromAtlasTexCoordsAndAddToRenderableAnimations
(
    const int meshAtlasCol,
    const int meshAtlasRow,
    const int atlasCols,
    const int atlasRows,
    const bool horFlipped,
    const std::string& modelName,
    const StringId animationNameId,
    RenderableComponent& renderableComponent
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* ModelUtils_h */
