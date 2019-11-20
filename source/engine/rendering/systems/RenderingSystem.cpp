///------------------------------------------------------------------------------------------------
///  RenderingSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#include "RenderingSystem.h"
#include "../components/CameraSingletonComponent.h"
#include "../components/RenderableComponent.h"
#include "../components/RenderingContextSingletonComponent.h"
#include "../components/ShaderStoreSingletonComponent.h"
#include "../components/WindowSingletonComponent.h"
#include "../opengl/Context.h"
#include "../utils/CameraUtils.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/FileUtils.h"
#include "../../common/utils/Logging.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/OSMessageBox.h"
#include "../../resources/MeshResource.h"
#include "../../resources/ResourceLoadingService.h"
#include "../../resources/TextureResource.h"
#include "../../sound/SoundService.h"

#include <algorithm> // sort
#include <cstdlib>   // exit
#include <SDL.h> 
#include <vector>
#include <iterator>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace
{
    const StringId WORLD_MARIX_UNIFORM_NAME      = StringId("world");
    const StringId VIEW_MARIX_UNIFORM_NAME       = StringId("view");
    const StringId PROJECTION_MARIX_UNIFORM_NAME = StringId("proj");    
}

///-----------------------------------------------------------------------------------------------

bool IsMeshInsideCameraFrustum
(
    const glm::vec3& meshPosition,
    const glm::vec3& meshScale,
    const glm::vec3& meshDimensions,
    const CameraFrustum& cameraFrustum
);

///-----------------------------------------------------------------------------------------------

RenderingSystem::RenderingSystem(genesis::ecs::World& world)
    : genesis::ecs::BaseSystem(world)
{
    InitializeRenderingWindowAndContext();
    InitializeCamera();
    CompileAndLoadShaders();
    CalculateAndSetComponentUsageMask<RenderableComponent, TransformComponent>();
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::VUpdateAssociatedComponents(const float) const
{
    // Get common rendering singleton components
    const auto& windowComponent               = mWorld.GetSingletonComponent<WindowSingletonComponent>();
    const auto& shaderStoreComponent          = mWorld.GetSingletonComponent<ShaderStoreSingletonComponent>();        
    auto& cameraComponent                     = mWorld.GetSingletonComponent<CameraSingletonComponent>();
    auto& renderingContextComponent           = mWorld.GetSingletonComponent<RenderingContextSingletonComponent>();    
    
    // Calculate render-constant camera view matrix
    cameraComponent.mViewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mFocusPosition, cameraComponent.mUpVector);
    
    // Calculate the camera frustum for this frame
    cameraComponent.mFrustum = CalculateCameraFrustum(cameraComponent.mViewMatrix, cameraComponent.mProjectionMatrix);
    
    // Collect all entities that need to be processed
    auto activeEntities = mWorld.GetActiveEntities();
    std::vector<genesis::ecs::EntityId> guiEntities;    
    
    // Set background color
    GL_CHECK(glClearColor
    (
        renderingContextComponent.mClearColor.x,
        renderingContextComponent.mClearColor.y,
        renderingContextComponent.mClearColor.z,
        renderingContextComponent.mClearColor.w
    ));
    
    // Clear buffers
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // Enable depth
    GL_CHECK(glEnable(GL_DEPTH_TEST));
        
    // Sort entities based on their depth order to correct transparency
    std::sort(activeEntities.begin(), activeEntities.end(), [this](const genesis::ecs::EntityId& lhs, const genesis::ecs::EntityId& rhs)
    {
        const auto& lhsTransformComponent = mWorld.GetComponent<TransformComponent>(lhs);
        const auto& rhsTransformComponent = mWorld.GetComponent<TransformComponent>(rhs);

        return lhsTransformComponent.mPosition.z > rhsTransformComponent.mPosition.z;
    });

    for (const auto& entityId : activeEntities)
    {
        if (ShouldProcessEntity(entityId))
        {
            const auto& renderableComponent = mWorld.GetComponent<RenderableComponent>(entityId);
            if (renderableComponent.mIsGuiComponent)
            {
                guiEntities.push_back(entityId);
                continue;
            }            
            else
            {            
                const auto& transformComponent = mWorld.GetComponent<TransformComponent>(entityId);
                const auto& activeMeshes       = renderableComponent.mAnimationsToMeshes.at(renderableComponent.mActiveAnimationNameId);
                const auto& currentMesh        = ResourceLoadingService::GetInstance().GetResource<MeshResource>(activeMeshes[renderableComponent.mActiveMeshIndex]);

                // Frustum culling
                if (!IsMeshInsideCameraFrustum
                (
                    transformComponent.mPosition,
                    transformComponent.mScale,
                    currentMesh.GetDimensions(),
                    cameraComponent.mFrustum
                ))
                {                    
                    continue;
                }

                RenderEntityInternal
                (
                    entityId,
                    renderableComponent,                    
                    cameraComponent,
                    shaderStoreComponent,
                    windowComponent,                    
                    renderingContextComponent
                );
            }            
        }
    }
    
    // Execute GUI render pass
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    
    for (const auto& entityId : guiEntities)
    {
        const auto& renderableComponent = mWorld.GetComponent<RenderableComponent>(entityId);
        RenderEntityInternal
        (
            entityId,
            renderableComponent,            
            cameraComponent,
            shaderStoreComponent,
            windowComponent,            
            renderingContextComponent
        );
    }

    // Swap window buffers
    SDL_GL_SwapWindow(windowComponent.mWindowHandle);
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::RenderEntityInternal
(
    const genesis::ecs::EntityId entityId,
    const RenderableComponent& renderableComponent,    
    const CameraSingletonComponent& cameraComponent, 
    const ShaderStoreSingletonComponent& shaderStoreComponent,
    const WindowSingletonComponent& windowComponent,    
    RenderingContextSingletonComponent& renderingContextComponent    
) const
{
    if (!renderableComponent.mVisibility)
    {
        return;
    }

    // Update Shader is necessary
    const ShaderResource* currentShader = nullptr;
    if (renderableComponent.mShaderNameId != renderingContextComponent.previousShaderNameId)
    {
        currentShader = &shaderStoreComponent.mShaders.at(renderableComponent.mShaderNameId);
        GL_CHECK(glUseProgram(currentShader->GetProgramId()));

        renderingContextComponent.previousShaderNameId = renderableComponent.mShaderNameId;
        renderingContextComponent.previousShader       = currentShader;
    }
    else
    {
        currentShader = renderingContextComponent.previousShader;
    }

    // Get currently active mesh for this entity
    const auto& transformComponent = mWorld.GetComponent<TransformComponent>(entityId);    
    const auto& activeMeshes       = renderableComponent.mAnimationsToMeshes.at(renderableComponent.mActiveAnimationNameId);

    // Update current mesh if necessary
    const MeshResource* currentMesh = nullptr;
    if (activeMeshes[renderableComponent.mActiveMeshIndex] != renderingContextComponent.previousMeshResourceId)
    {
        currentMesh = &ResourceLoadingService::GetInstance().GetResource<MeshResource>(activeMeshes[renderableComponent.mActiveMeshIndex]);
        GL_CHECK(glBindVertexArray(currentMesh->GetVertexArrayObject()));

        renderingContextComponent.previousMesh           = currentMesh;
        renderingContextComponent.previousMeshResourceId = activeMeshes[renderableComponent.mActiveMeshIndex];
    }
    else
    {
        currentMesh = renderingContextComponent.previousMesh;
    }
        
    // Calculate world matrix for entity
    glm::mat4 world(1.0f);
        
    // Correct display of hud and billboard entities    
    glm::vec3 position = transformComponent.mPosition;
    glm::vec3 scale    = transformComponent.mScale;
    glm::vec3 rotation = transformComponent.mRotation;

    if (renderableComponent.mIsGuiComponent)
    {        
        scale.x /= windowComponent.mAspectRatio;        
    }  

    world = glm::translate(world, position);
    world = glm::rotate(world, rotation.x, math::X_AXIS);
    world = glm::rotate(world, rotation.y, math::Y_AXIS);
    world = glm::rotate(world, rotation.z, math::Z_AXIS);
    world = glm::scale(world, scale);
        
    // Update texture if necessary
    const TextureResource* currentTexture = nullptr;
    if (renderableComponent.mTextureResourceId != renderingContextComponent.previousTextureResourceId)
    {
        currentTexture = &ResourceLoadingService::GetInstance().GetResource<TextureResource>(renderableComponent.mTextureResourceId);
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, currentTexture->GetGLTextureId()));

        renderingContextComponent.previousTexture = currentTexture;
        renderingContextComponent.previousTextureResourceId = renderableComponent.mTextureResourceId;
    }
    else
    {
        currentTexture = renderingContextComponent.previousTexture;
    }      

    // Set uniforms    
    GL_CHECK(glUniformMatrix4fv(currentShader->GetUniformNamesToLocations().at(WORLD_MARIX_UNIFORM_NAME), 1, GL_FALSE, (GLfloat*)&world));
    GL_CHECK(glUniformMatrix4fv(currentShader->GetUniformNamesToLocations().at(VIEW_MARIX_UNIFORM_NAME), 1, GL_FALSE, (GLfloat*)&cameraComponent.mViewMatrix));
    GL_CHECK(glUniformMatrix4fv(currentShader->GetUniformNamesToLocations().at(PROJECTION_MARIX_UNIFORM_NAME), 1, GL_FALSE, (GLfloat*)&cameraComponent.mProjectionMatrix));        
    
    // Perform draw call
    GL_CHECK(glDrawElements(GL_TRIANGLES, currentMesh->GetElementCount(), GL_UNSIGNED_SHORT, (void*)0));
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::InitializeRenderingWindowAndContext() const
{    
    // Create SDL GL context
    auto renderingContextComponent = std::make_unique<RenderingContextSingletonComponent>();
    auto& windowComponent = mWorld.GetSingletonComponent<WindowSingletonComponent>();

    renderingContextComponent->mGLContext = SDL_GL_CreateContext(windowComponent.mWindowHandle);
    if (renderingContextComponent->mGLContext == nullptr)
    {
        ShowMessageBox(MessageBoxType::ERROR, "Error creating SDL context", "An error has occurred while trying to create an SDL_Context");
        exit(1);
    }

    // Commit context 
    SDL_GL_MakeCurrent(windowComponent.mWindowHandle, renderingContextComponent->mGLContext);
    SDL_GL_SetSwapInterval(0);

#ifdef _WIN32
    // Initialize GLES2 function table
    glFuncTable.initialize();
#endif

    // Get actual render buffer width/height
    auto renderableWidth  = 0;
    auto renderableHeight = 0;
    SDL_GL_GetDrawableSize(windowComponent.mWindowHandle, &renderableWidth, &renderableHeight);

    windowComponent.mRenderableWidth  = static_cast<float>(renderableWidth);
    windowComponent.mRenderableHeight = static_cast<float>(renderableHeight);
    windowComponent.mAspectRatio      = windowComponent.mRenderableWidth/windowComponent.mRenderableHeight;
    
    // Log GL driver info
    Log(LogType::INFO, "Vendor     : %s", GL_NO_CHECK(glGetString(GL_VENDOR)));
    Log(LogType::INFO, "Renderer   : %s", GL_NO_CHECK(glGetString(GL_RENDERER)));
    Log(LogType::INFO, "Version    : %s", GL_NO_CHECK(glGetString(GL_VERSION)));   

    // Configure Blending
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));    
    
    // Configure Depth
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(GL_LESS));    
    
    // Transfer ownership of singleton components to world    
    mWorld.SetSingletonComponent<RenderingContextSingletonComponent>(std::move(renderingContextComponent));    
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::InitializeCamera() const
{
    const auto& windowComponent = mWorld.GetSingletonComponent<WindowSingletonComponent>();
    auto cameraComponent = std::make_unique<CameraSingletonComponent>();
    cameraComponent->mProjectionMatrix = glm::perspectiveFovLH
    (
        cameraComponent->mFieldOfView,
        windowComponent.mRenderableWidth,
        windowComponent.mRenderableHeight,
        cameraComponent->mZNear,
        cameraComponent->mZFar
    );
    
    mWorld.SetSingletonComponent<CameraSingletonComponent>(std::move(cameraComponent));
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::CompileAndLoadShaders() const
{
    auto& renderingContextComponent = mWorld.GetSingletonComponent<RenderingContextSingletonComponent>();
    
    // Bind default VAO for correct shader compilation
    GL_CHECK(glGenVertexArrays(1, &renderingContextComponent.mDefaultVertexArrayObject));
    GL_CHECK(glBindVertexArray(renderingContextComponent.mDefaultVertexArrayObject));
    
    const auto shaderNames    = GetAndFilterShaderNames();
    auto shaderStoreComponent = std::make_unique<ShaderStoreSingletonComponent>();
    
    for (const auto& shaderName: shaderNames)
    {
        // By signaling to load either a .vs or a .fs, the ShaderLoader will load the pair automatically,
        // hence why the addition of the .vs here
        auto shaderResourceId = ResourceLoadingService::GetInstance().LoadResource(ResourceLoadingService::RES_SHADERS_ROOT + shaderName + ".vs");
        auto& shaderResource  = ResourceLoadingService::GetInstance().GetResource<ShaderResource>(shaderResourceId);
        
        // Save a copy of the shader to the ShaderStoreComponent
        shaderStoreComponent->mShaders[StringId(shaderName)] = shaderResource;
        
        // And unload the resource
        ResourceLoadingService::GetInstance().UnloadResource(shaderResourceId);
    }
    
    // Unbind any VAO currently bound
    GL_CHECK(glBindVertexArray(0));
    
    mWorld.SetSingletonComponent<ShaderStoreSingletonComponent>(std::move(shaderStoreComponent));
}

///-----------------------------------------------------------------------------------------------

std::set<std::string> RenderingSystem::GetAndFilterShaderNames() const
{
    const auto vertexAndFragmentShaderFilenames = GetAllFilenamesInDirectory(ResourceLoadingService::RES_SHADERS_ROOT);

    std::set<std::string> shaderNames;
    for (const auto& shaderFilename : vertexAndFragmentShaderFilenames)
    {
        shaderNames.insert(GetFileNameWithoutExtension(shaderFilename));
    }
    return shaderNames;
}

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

}

