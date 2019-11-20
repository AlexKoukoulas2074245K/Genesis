///------------------------------------------------------------------------------------------------
///  CameraSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef CameraSingletonComponent_h
#define CameraSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"

#include <array>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace 
{
    const unsigned int CAMERA_FRUSTUM_SIDES = 6;

    const glm::vec3 DEFAULT_CAMERA_FOCUS_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    const glm::vec3 DEFAULT_CAMERA_POSITION       = glm::vec3(0.0f, 18.04f, 0.0f);
    const glm::vec3 DEFAULT_CAMERA_UP_VECTOR      = math::Y_AXIS;

    const float DEFAULT_CAMERA_FIELD_OF_VIEW = math::PI / 2.0f;
    const float DEFAULT_CAMERA_Z_NEAR        = 0.1f;
    const float DEFAULT_CAMERA_Z_FAR         = 100.0f;
}


///-----------------------------------------------------------------------------------------------

using CameraFrustum = std::array<glm::vec4, CAMERA_FRUSTUM_SIDES>;

///-----------------------------------------------------------------------------------------------

class CameraSingletonComponent final: public ecs::IComponent
{
public:
    CameraFrustum mFrustum;
    glm::mat4 mViewMatrix       = glm::mat4(1.0f);
    glm::mat4 mProjectionMatrix = glm::mat4(1.0f);    
    glm::vec3 mUpVector         = DEFAULT_CAMERA_UP_VECTOR;
    glm::vec3 mPosition         = DEFAULT_CAMERA_POSITION;
    glm::vec3 mFocusPosition    = DEFAULT_CAMERA_FOCUS_POSITION;
    float mFieldOfView          = DEFAULT_CAMERA_FIELD_OF_VIEW;
    float mZNear                = DEFAULT_CAMERA_Z_NEAR;
    float mZFar                 = DEFAULT_CAMERA_Z_FAR;
};

///-----------------------------------------------------------------------------------------------

}

#endif /* CameraSingletonComponent_h */
