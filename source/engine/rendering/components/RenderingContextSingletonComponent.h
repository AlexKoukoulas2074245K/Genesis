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
    SDL_GLContext mGLContext            = nullptr;
    GLuint mDefaultVertexArrayObject    = 0;    
    unsigned int mFrustumCulledEntities = 0;
    unsigned int mRenderedEntities      = 0;
    bool mBlending                      = false;
    bool mDepthTest                     = false;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderingContextSingletonComponent_h */
