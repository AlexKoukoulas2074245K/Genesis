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

using ResourceId = unsigned int;

///-----------------------------------------------------------------------------------------------

class RenderableComponent final: public ecs::IComponent
{
public:
    // This should be a map of vectors (animation name to meshes)
    std::unordered_map<StringId, std::vector<ResourceId>, StringIdHasher> mAnimationsToMeshes;    
    StringId mShaderNameId                 = StringId();    
    StringId mActiveAnimationNameId        = StringId();
    size_t mActiveMeshIndex                = 0;
    ResourceId mTextureResourceId          = 0;            
    bool mIsGuiComponent                   = false;
    bool mVisibility                       = true;    
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderableComponent_h */
