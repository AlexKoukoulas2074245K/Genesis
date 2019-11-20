///------------------------------------------------------------------------------------------------
///  CameraUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef CameraUtils_h
#define CameraUtils_h

///-----------------------------------------------------------------------------------------------

#include "../../common/utils/MathUtils.h"
#include "../components/CameraSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

CameraFrustum CalculateCameraFrustum(const glm::mat4& viewMatrix, const glm::mat4& projMatrix);

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* CameraUtis_h */
