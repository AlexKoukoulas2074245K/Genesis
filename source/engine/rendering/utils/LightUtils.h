///------------------------------------------------------------------------------------------------
///  LightUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 27/07/2020.
///------------------------------------------------------------------------------------------------

#ifndef LightUtils_h
#define LightUtils_h

///------------------------------------------------------------------------------------------------

#include "../../common/utils/MathUtils.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------
/// Adds a light source to the world.
///
/// @param[in] lightPosition the position of the light
void AddLightSource
(
    const glm::vec3& lightPosition
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* FontUtils_h */
