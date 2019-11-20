///------------------------------------------------------------------------------------------------
///  RenderingSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef RenderingSystem_h
#define RenderingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../common/utils/StringUtils.h"
#include "../../common/utils/MathUtils.h"
#include "../../ECS.h"

#include <set>
#include <string>
#include <unordered_set>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

class CameraSingletonComponent;
class RenderableComponent;
class RenderingContextSingletonComponent;
class ShaderStoreSingletonComponent;
class WindowSingletonComponent;

///-----------------------------------------------------------------------------------------------

class RenderingSystem final: public ecs::BaseSystem
{
public:
    RenderingSystem(ecs::World& world);
    
    void VUpdateAssociatedComponents(const float dt) const override;

private:
    void RenderEntityInternal
    (
        const genesis::ecs::EntityId entityId,
        const RenderableComponent& entityRenderableComponent,        
        const CameraSingletonComponent& globalCameraComponent,
        const ShaderStoreSingletonComponent& globalShaderStoreComponent,
        const WindowSingletonComponent& globalWindowComponent,        
        RenderingContextSingletonComponent& renderingContextComponent        
    ) const;
        
    void InitializeRenderingWindowAndContext() const;
    void InitializeCamera() const;
    void CompileAndLoadShaders() const;

    std::set<std::string> GetAndFilterShaderNames() const;

};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderingSystem_h */
