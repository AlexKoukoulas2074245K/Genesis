///------------------------------------------------------------------------------------------------
///  RenderingContextSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef RenderingContextSingletonComponent_h
#define RenderingContextSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/StringUtils.h"
#include "../../resources/MeshResource.h"
#include "../../resources/ResourceLoadingService.h"
#include "../../resources/ShaderResource.h"
#include "../../resources/TextureResource.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

using SDL_GLContext = void*;
using GLuint        = unsigned int;

///-----------------------------------------------------------------------------------------------

class RenderingContextSingletonComponent final : public ecs::IComponent
{
public:        
    // Core state
    SDL_GLContext mGLContext            = nullptr;
    GLuint mDefaultVertexArrayObject    = 0;        
    glm::vec4 mClearColor               = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    // Previous render call resource pointers
    const res::ShaderResource* previousShader   = nullptr;
    const res::TextureResource* previousTexture = nullptr;
    const res::MeshResource* previousMesh       =  nullptr;

    // Previous render call resource ids
    StringId previousShaderNameId             = StringId();
    res::ResourceId previousTextureResourceId = ResourceId();
    res::ResourceId previousMeshResourceId    = ResourceId();
    
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderingContextSingletonComponent_h */
