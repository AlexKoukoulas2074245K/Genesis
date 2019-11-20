///------------------------------------------------------------------------------------------------
///  RenderingUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#include "RenderingUtils.h"
#include "../components/RenderableComponent.h"
#include "../../services/ResourceLoadingService.h"
#include "../../resources/TextureResource.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

bool IsMeshInsideCameraFrustum
(
    const glm::vec3& meshPosition,
    const glm::vec3& meshScale,
    const glm::vec3& meshDimensions, 
    const CameraFrustum& cameraFrustum
)
{    
    const auto scaledMeshDimensions = meshDimensions * meshScale;
    const auto frustumCheckSphereRadius = math::Max(scaledMeshDimensions.x, math::Max(scaledMeshDimensions.y, scaledMeshDimensions.z));

    for (auto i = 0U; i < 6U; ++i)
    {
        float dist = 
            cameraFrustum[i].x * meshPosition.x +
            cameraFrustum[i].y * meshPosition.y +
            cameraFrustum[i].z * meshPosition.z +
            cameraFrustum[i].w - frustumCheckSphereRadius;

        if (dist > 0) return false;
    }
  
    return true;
}

///-----------------------------------------------------------------------------------------------

}

