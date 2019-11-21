///------------------------------------------------------------------------------------------------
///  ModelUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "ModelUtils.h"
#include "../components/RenderableComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/MathUtils.h"

#include <vector>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateModelByName
(
    const std::string& modelName,
    const glm::vec3& initialPosition,
    ecs::World& world
)
{
    const auto modelEntity = world.CreateEntity();

    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = initialPosition;

    auto renderableComponent = std::make_unique<RenderableComponent>();    
    renderableComponent->mActiveAnimationNameId = StringId("default");        
    renderableComponent->mShaderNameId = StringId("basic");

    renderableComponent->mAnimationsToMeshes[StringId("default")].push_back
    (
        res::ResourceLoadingService::GetInstance().
        LoadResource(res::ResourceLoadingService::RES_MODELS_ROOT + modelName + ".obj"
    ));
        
    renderableComponent->mTextureResourceId = res::ResourceLoadingService::GetInstance().LoadResource
    (
        res::ResourceLoadingService::RES_TEXTURES_ROOT + modelName + ".png"
    );
    
    world.AddComponent<RenderableComponent>(modelEntity, std::move(renderableComponent));
    world.AddComponent<TransformComponent>(modelEntity, std::move(transformComponent));

    return modelEntity;
}

///------------------------------------------------------------------------------------------------

static std::vector<glm::vec2> CalculateTextureCoordsFromColumnAndRow
(
    const int col,
    const int row,
    const int atlasNumberOfCols,
    const int atlasNumberOfRows,
    const bool horFlipped
);

///------------------------------------------------------------------------------------------------

static std::string CreateTexCoordInjectedModelPath
(
    const std::string& modelName,
    const std::vector<glm::vec2>& texCoords
);

///------------------------------------------------------------------------------------------------

ResourceId LoadMeshFromAtlasTexCoords
(
    const int meshAtlasCol,
    const int meshAtlasRow,
    const int atlasCols,
    const int atlasRows,
    const bool horFlipped,
    const std::string& modelName
)
{
    auto correctedMeshCol = meshAtlasCol;
    auto correctedMeshRow = meshAtlasRow;

    if (correctedMeshCol >= atlasCols)
    {
        correctedMeshCol %= atlasCols;
        correctedMeshRow++;
    }

    const auto texCoords = CalculateTextureCoordsFromColumnAndRow(correctedMeshCol, correctedMeshRow, atlasCols, atlasRows, horFlipped);
    const auto meshPath  = CreateTexCoordInjectedModelPath(modelName, texCoords);

    const auto loadedMeshResourceId = res::ResourceLoadingService::GetInstance().LoadResource(meshPath);
    return loadedMeshResourceId;
}

///------------------------------------------------------------------------------------------------

ResourceId LoadMeshFromAtlasTexCoordsAndAddToRenderableAnimations
(
    const int meshAtlasCol,
    const int meshAtlasRow, 
    const int atlasCols,
    const int atlasRows,
    const bool horFlipped,
    const std::string& modelName,
    const StringId animationNameId,
    RenderableComponent& renderableComponent
)
{
    const auto loadedMeshResourceId = LoadMeshFromAtlasTexCoords(meshAtlasCol, meshAtlasRow, atlasCols, atlasRows, horFlipped, modelName);
    renderableComponent.mAnimationsToMeshes[animationNameId].push_back(loadedMeshResourceId);
    return loadedMeshResourceId;
}

///------------------------------------------------------------------------------------------------

std::vector<glm::vec2> CalculateTextureCoordsFromColumnAndRow
(
    const int col,
    const int row,
    const int atlasNumberOfCols,
    const int atlasNumberOfRows,
    const bool horFlipped
)
{
    const auto cols       = static_cast<float>(atlasNumberOfCols);
    const auto rows       = static_cast<float>(atlasNumberOfRows);
    const auto cellWidth  = 1.0f/cols;    
    const auto cellHeight = 1.0f/rows;

    if (horFlipped)
    {
        return
        {
            glm::vec2((col + 1) * cellWidth, 1.0f - row * cellHeight),
            glm::vec2(col * cellWidth,       1.0f - row * cellHeight),
            glm::vec2(col * cellWidth,       1.0f - (row + 1) * cellHeight),
            glm::vec2((col + 1) * cellWidth, 1.0f - (row + 1) * cellHeight)
        };
    }
    else
    {
        return
        {
            glm::vec2(col * cellWidth,       1.0f - row * cellHeight),
            glm::vec2((col + 1) * cellWidth, 1.0f - row * cellHeight),
            glm::vec2((col + 1) * cellWidth, 1.0f - (row + 1) * cellHeight),
            glm::vec2(col * cellWidth,       1.0f - (row + 1) * cellHeight)
        };
    }    
}

///------------------------------------------------------------------------------------------------

std::string CreateTexCoordInjectedModelPath
(
    const std::string& modelName,
    const std::vector<glm::vec2>& texCoords
)
{
    std::string path = res::ResourceLoadingService::RES_MODELS_ROOT + modelName + "[";

    path += std::to_string(texCoords[0].x) + "," + std::to_string(texCoords[0].y);

    for (auto i = 1U; i < texCoords.size(); ++i)
    {
        path += "-" + std::to_string(texCoords[i].x) + "," + std::to_string(texCoords[i].y);
    }

    path += "].obj";

    return path;
}

///------------------------------------------------------------------------------------------------

}

