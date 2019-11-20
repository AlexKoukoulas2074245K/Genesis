///------------------------------------------------------------------------------------------------
///  PreviousRenderingStateSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef PreviousRenderingStateSingletonComponent_h
#define PreviousRenderingStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/StringUtils.h"
#include "../../services/ResourceLoadingService.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

class ShaderResource;
class TextureResource;
class MeshResource;

///-----------------------------------------------------------------------------------------------

class PreviousRenderingStateSingletonComponent final : public ecs::IComponent
{
public:
    StringId previousShaderNameId        = StringId();
    ResourceId previousTextureResourceId = ResourceId();
    ResourceId previousMeshResourceId    = ResourceId();

    const ShaderResource* previousShader       = nullptr;
    const TextureResource* previousTexture     = nullptr;
    const MeshResource* previousMesh           = nullptr;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* PreviousRenderingStateSingletonComponent_h */