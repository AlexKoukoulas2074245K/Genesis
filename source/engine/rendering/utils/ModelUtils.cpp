///------------------------------------------------------------------------------------------------
///  ModelUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "ModelUtils.h"
#include "../components/RenderableComponent.h"
#include "../../common/components/NameComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/MathUtils.h"

#include <vector>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------

namespace
{
    const StringId DEFAULT_MODEL_SHADER = StringId("default_3d");    

}

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateModelByName
(
    const std::string& modelName,
    const glm::vec3& initialPosition,
    ecs::World& world,
    const StringId entityName /* StringId() */
)
{
    const auto modelEntity = world.CreateEntity();

    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = initialPosition;

    auto renderableComponent = std::make_unique<RenderableComponent>();    
    renderableComponent->mActiveAnimationNameId = StringId("default");        
    renderableComponent->mShaderNameId = DEFAULT_MODEL_SHADER;

    renderableComponent->mAnimationsToMeshes[StringId("default")].push_back
    (
        resources::ResourceLoadingService::GetInstance().
        LoadResource(resources::ResourceLoadingService::RES_MODELS_ROOT + modelName + ".obj"
    ));
        
    renderableComponent->mTextureResourceId = resources::ResourceLoadingService::GetInstance().LoadResource
    (
        resources::ResourceLoadingService::RES_TEXTURES_ROOT + modelName + ".png"
    );
    
    world.AddComponent<RenderableComponent>(modelEntity, std::move(renderableComponent));
    world.AddComponent<TransformComponent>(modelEntity, std::move(transformComponent));

    if (entityName != StringId())
    {
        world.AddComponent<NameComponent>(modelEntity, std::make_unique<NameComponent>(entityName));
    }

    return modelEntity;
}

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateGuiSprite
(
    const std::string& modelName,
    const std::string& textureName,
    const StringId shaderName,
    const glm::vec3& initialPosition,
    ecs::World& world,
    const StringId entityName /* StringId() */
)
{
    const auto modelEntity = world.CreateEntity();

    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = initialPosition;

    auto renderableComponent = std::make_unique<RenderableComponent>();
    renderableComponent->mActiveAnimationNameId = StringId("default");
    renderableComponent->mShaderNameId = shaderName;
    renderableComponent->mIsGuiComponent = true;
    renderableComponent->mAnimationsToMeshes[StringId("default")].push_back
    (
        resources::ResourceLoadingService::GetInstance().
        LoadResource(resources::ResourceLoadingService::RES_MODELS_ROOT + modelName + ".obj"
    ));

    renderableComponent->mTextureResourceId = resources::ResourceLoadingService::GetInstance().LoadResource
    (
        resources::ResourceLoadingService::RES_TEXTURES_ROOT + textureName + ".png"
    );

    world.AddComponent<RenderableComponent>(modelEntity, std::move(renderableComponent));
    world.AddComponent<TransformComponent>(modelEntity, std::move(transformComponent));

    if (entityName != StringId())
    {
        world.AddComponent<NameComponent>(modelEntity, std::make_unique<NameComponent>(entityName));
    }

    return modelEntity;
}

///------------------------------------------------------------------------------------------------

}

}

