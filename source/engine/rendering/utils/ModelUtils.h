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
    ecs::World& world,
    const StringId entityName = StringId()
);

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateGuiSprite
(
    const std::string& modelName,
    const std::string& textureName,
    const StringId shaderName,
    const glm::vec3& initialPosition,
    ecs::World& world,
    const StringId entityName = StringId()
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* ModelUtils_h */
