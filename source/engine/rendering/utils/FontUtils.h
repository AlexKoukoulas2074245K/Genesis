///------------------------------------------------------------------------------------------------
///  FontUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///------------------------------------------------------------------------------------------------

#ifndef FontUtils_h
#define FontUtils_h

///------------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------
/// Loads the font with the given name. 
///
/// This function assumes that a font texture atlas (res/textures/atlases) and a font data file
/// (under res/data/font_maps) exist with the same name as the one passed in the function.
/// @param[in] fontName the name of the font to load.
/// @param[in] fontAtlasCols the number of columns in the font atlas texture.
/// @param[in] fontAtlasRows the number of rows in the font atlas texture.
void LoadFont
(
    const StringId fontName, 
    const int fontAtlasCols, 
    const int fontAtlasRows
);

///------------------------------------------------------------------------------------------------
/// Renders a single character with the given font. 
///
/// @param[in] character the character to render.
/// @param[in] fontName the name of the font to use in rendering.
/// @param[in] size the size of character.
/// @param[in] position the position to render the character at.
/// @param[in] color (optional) specifies the custom color of the rendered character.
/// @returns the id of an entity holding the renderable and transform components of the character.
ecs::EntityId RenderCharacter
(
    const char character,
    const StringId fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
);

///------------------------------------------------------------------------------------------------
/// Renders a text string with the given font. 
///
/// @param[in] text the text to render.
/// @param[in] fontName the name of the font to use in the text rendering.
/// @param[in] size the size of the rendered text's individual glyphs.
/// @param[in] position the position to render the string at.
/// @param[in] color (optional) specifies the custom color of the rendered string.
/// @returns the id of an entity holding the root TextStringComponent which contains all the character entities of the input string.
ecs::EntityId RenderText
(
    const std::string& text,
    const StringId fontName,
    const float size,
    const glm::vec3& position,
    const glm::vec4& color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
);

///------------------------------------------------------------------------------------------------
/// Clears a text string. 
///
/// @param[in] textStringEntityId the id of the entity holding the root TextStringComponent of the text to be cleared.
void ClearRenderedText
(
    const ecs::EntityId textStringEntityId
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* FontUtils_h */
