///------------------------------------------------------------------------------------------------
///  RenderableComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef RenderableComponent_h
#define RenderableComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/StringUtils.h"

#include <unordered_map>
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

using ResourceId = unsigned int;

///-----------------------------------------------------------------------------------------------

struct ShaderUniforms final
{
    std::unordered_map<StringId, float, StringIdHasher> mShaderFloatUniforms;
    std::unordered_map<StringId, glm::mat4, StringIdHasher> mShaderMatrixUniforms;
};

///-----------------------------------------------------------------------------------------------

class RenderableComponent final: public ecs::IComponent
{
public:    
    std::unordered_map<StringId, std::vector<ResourceId>, StringIdHasher> mAnimationsToMeshes;    
    ShaderUniforms mShaderUniforms;
    StringId mShaderNameId                 = StringId();    
    StringId mActiveAnimationNameId        = StringId();
    size_t mActiveMeshIndex                = 0;
    ResourceId mTextureResourceId          = 0;            
    bool mIsGuiComponent                   = false;
    bool mVisibility                       = true;    
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderableComponent_h */
