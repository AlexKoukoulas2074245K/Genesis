///------------------------------------------------------------------------------------------------
///  FontUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///------------------------------------------------------------------------------------------------

#include "FontUtils.h"
#include "MeshUtils.h"
#include "../components/FontsStoreSingletonComponent.h"
#include "../components/RenderableComponent.h"
#include "../components/TextStringComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../resources/ResourceLoadingService.h"
#include "../../resources/DataFileResource.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId FONT_SHADER_NAME                     = StringId("default_gui");
    static const StringId GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME = StringId("custom_color");

    static const std::string FONT_MAP_FILE_EXTENSION           = ".dat";
    static const std::string FONT_ATLAS_TEXTURE_FILE_EXTENSION = ".png";

    static const float FONT_PADDING_PROPORTION_TO_SIZE = 0.333333f;
}

///------------------------------------------------------------------------------------------------

void LoadFont
(
    const StringId fontName,
    const int fontAtlasCols,
    const int fontAtlasRows
)
{
    auto& world = ecs::World::GetInstance();

    // Get/Create the font store component
    if (!world.HasSingletonComponent<FontsStoreSingletonComponent>())
    {
        world.SetSingletonComponent<FontsStoreSingletonComponent>(std::make_unique<FontsStoreSingletonComponent>());
    }

    // Make sure the font is not already loaded
    auto& fontStoreComponent = world.GetSingletonComponent<FontsStoreSingletonComponent>();
    if (fontStoreComponent.mLoadedFonts.count(fontName) != 0)
    {
        return;
    }

    const auto fontMapFileResourceId = resources::ResourceLoadingService::GetInstance().LoadResource(resources::ResourceLoadingService::RES_FONT_MAP_DATA_ROOT + fontName.GetString() + FONT_MAP_FILE_EXTENSION);
    const auto& fontMapFileResource  = resources::ResourceLoadingService::GetInstance().GetResource<resources::DataFileResource>(fontMapFileResourceId);

    const auto fontMapSplitByNewline = StringSplit(fontMapFileResource.GetContents(), '\n');
    
    for (auto row = 0U; row < fontMapSplitByNewline.size(); ++row)
    {
        const auto fontMapLineSplitBySpace = StringSplit(fontMapSplitByNewline[row], ' ');
        for (auto col = 0U; col < fontMapLineSplitBySpace.size(); ++col)
        {
            const auto currentFontCharacter = fontMapLineSplitBySpace[col][0];
            fontStoreComponent.mLoadedFonts[fontName][currentFontCharacter] = LoadAndCreateMeshFromAtlasTexCoords
            (
                col,
                row,
                fontAtlasCols,
                fontAtlasRows,
                false                
            );
        }
    }

    resources::ResourceLoadingService::GetInstance().UnloadResource(fontMapFileResourceId);
}

///-----------------------------------------------------------------------------------------------

ecs::EntityId RenderCharacter
(
    const char character,
    const StringId fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color /* glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) */
)
{    
    auto& world = ecs::World::GetInstance();
    auto& fontStoreComponent = world.GetSingletonComponent<FontsStoreSingletonComponent>();

    auto renderableComponent = std::make_unique<RenderableComponent>();
    renderableComponent->mTextureResourceId = resources::ResourceLoadingService::GetInstance().LoadResource(resources::ResourceLoadingService::RES_ATLASES_ROOT + fontName.GetString() + FONT_ATLAS_TEXTURE_FILE_EXTENSION);
    renderableComponent->mActiveAnimationNameId = StringId("default");
    renderableComponent->mShaderNameId = FONT_SHADER_NAME;
    renderableComponent->mIsGuiComponent = true;
    renderableComponent->mAnimationsToMeshes[renderableComponent->mActiveAnimationNameId].push_back(fontStoreComponent.mLoadedFonts.at(fontName).at(character));
    renderableComponent->mShaderUniforms.mShaderFloatVec4Uniforms[GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME] = color;

    auto transformComponent = std::make_unique<TransformComponent>();    
    transformComponent->mPosition = position;
    transformComponent->mScale = glm::vec3(size);

    const auto characterEntity = world.CreateEntity();
    world.AddComponent<RenderableComponent>(characterEntity, std::move(renderableComponent));
    world.AddComponent<TransformComponent>(characterEntity, std::move(transformComponent));

    return characterEntity;
}

///-----------------------------------------------------------------------------------------------

ecs::EntityId RenderText
(
    const std::string& text,
    const StringId fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color /* glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) */
)
{
    auto& world = ecs::World::GetInstance();
    auto& fontStoreComponent = world.GetSingletonComponent<FontsStoreSingletonComponent>();
    auto textStringComponent = std::make_unique<TextStringComponent>();

    auto positionCounter = position;
    for (const auto& character : text)
    {
        if (character != ' ' && fontStoreComponent.mLoadedFonts.at(fontName).count(character) == 0)
        {
            continue;
        }

        // Don't add transform or model components for whitespace character
        if (character != ' ')
        {
            const auto characterEntityId = RenderCharacter(character, fontName, size, positionCounter, color);
            textStringComponent->mTextCharacterEntities.push_back(CharacterEntry(characterEntityId, character));
        }

        positionCounter.x += size * FONT_PADDING_PROPORTION_TO_SIZE;
    }    

    textStringComponent->mSize = size;

    auto entity = world.CreateEntity();
    world.AddComponent<TextStringComponent>(entity, std::move(textStringComponent));

    return entity;
}

///-----------------------------------------------------------------------------------------------

void ClearRenderedText
(
    const ecs::EntityId textStringEntityId
)
{
    auto& world = ecs::World::GetInstance();
    auto& textStringComponent = world.GetComponent<TextStringComponent>(textStringEntityId);

    for (const auto& characterEntity : textStringComponent.mTextCharacterEntities)
    {
        world.DestroyEntity(characterEntity.mEntityId);
    }

    textStringComponent.mTextCharacterEntities.clear();
    world.DestroyEntity(textStringEntityId);
}

///-----------------------------------------------------------------------------------------------

void MoveText
(
    const ecs::EntityId textStringEntityId,
    const float dx /* 0.0f */,
    const float dy /* 0.0f */
)
{
    auto& world = ecs::World::GetInstance();
    auto& textStringComponent = world.GetComponent<TextStringComponent>(textStringEntityId);
    
    for (const auto& characterEntity : textStringComponent.mTextCharacterEntities)
    {
        world.GetComponent<TransformComponent>(characterEntity.mEntityId).mPosition += glm::vec3(dx, dy, 0.0f);
    }
}
    
///-----------------------------------------------------------------------------------------------

void SetTextPosition
(
    const ecs::EntityId textStringEntityId,
    const glm::vec3& position
)
{
    auto& world = ecs::World::GetInstance();
    auto& textStringComponent = world.GetComponent<TextStringComponent>(textStringEntityId);

    auto positionCounter = position;
    for (auto i = 0U; i < textStringComponent.mTextCharacterEntities.size(); ++i)
    {
        const auto& characterEntry = textStringComponent.mTextCharacterEntities.at(i);
        world.GetComponent<TransformComponent>(characterEntry.mEntityId).mPosition = positionCounter;

        positionCounter.x += textStringComponent.mSize * FONT_PADDING_PROPORTION_TO_SIZE;
    }
}

///-----------------------------------------------------------------------------------------------

}

}

