//
//  RenderingSystem.cpp
//  Genesis
//
//  Created by Alex Koukoulas on 28/03/2019.
//

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

#include "RenderingSystem.h"
#include "../components/CameraSingletonComponent.h"
#include "../components/PreviousRenderingStateSingletonComponent.h"
#include "../components/RenderableComponent.h"
#include "../components/RenderingContextSingletonComponent.h"
#include "../components/ShaderStoreSingletonComponent.h"
#include "../components/WindowSingletonComponent.h"
#include "../opengl/Context.h"
#include "../utils/CameraUtils.h"
#include "../utils/RenderingUtils.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/FileUtils.h"
#include "../../common/utils/Logging.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/OSMessageBox.h"
#include "../../resources/MeshResource.h"
#include "../../resources/TextureResource.h"
#include "../../services/ResourceLoadingService.h"
#include "../../services/SoundService.h"

#include <algorithm> // sort
#include <cstdlib>   // exit
#include <SDL.h> 
#include <vector>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

namespace
{
    const StringId WORLD_MARIX_UNIFORM_NAME      = StringId("world");
    const StringId VIEW_MARIX_UNIFORM_NAME       = StringId("view");
    const StringId PROJECTION_MARIX_UNIFORM_NAME = StringId("proj");

    const float TARGET_ASPECT_RATIO = 1.5993266f;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

RenderingSystem::RenderingSystem(genesis::ecs::World& world)
    : genesis::ecs::BaseSystem(world)
{
    InitializeRenderingWindowAndContext();
    InitializeCamera();
    CompileAndLoadShaders();
    CalculateAndSetComponentUsageMask<RenderableComponent, TransformComponent>();
}

void RenderingSystem::VUpdateAssociatedComponents(const float) const
{
    // Get common rendering singleton components
    const auto& windowComponent               = mWorld.GetSingletonComponent<WindowSingletonComponent>();
    const auto& shaderStoreComponent          = mWorld.GetSingletonComponent<ShaderStoreSingletonComponent>();        
    auto& cameraComponent                     = mWorld.GetSingletonComponent<CameraSingletonComponent>();
    auto& renderingContextComponent           = mWorld.GetSingletonComponent<RenderingContextSingletonComponent>();
    auto& previousRenderingStateComponent     = mWorld.GetSingletonComponent<PreviousRenderingStateSingletonComponent>();
    
    // Calculate render-constant camera view matrix
    cameraComponent.mViewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mFocusPosition, cameraComponent.mUpVector);
    
    // Calculate the camera frustum for this frame
    cameraComponent.mFrustum = CalculateCameraFrustum(cameraComponent.mViewMatrix, cameraComponent.mProjectionMatrix);
        
    // Debug statistics
    renderingContextComponent.mFrustumCulledEntities = 0U;
    renderingContextComponent.mRenderedEntities      = 0U;
    
    // Collect all entities that need to be processed
    auto activeEntities = mWorld.GetActiveEntities();
    std::vector<genesis::ecs::EntityId> guiEntities;    
    
    // Set background color
    //GL_CHECK(glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w));
    
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
            /*
            // Separate GUI entities to render them last
            if (GUI_SHADERS.count(renderableComponent.mShaderNameId) != 0)
            {
                guiEntities.push_back(entityId);
                continue;
            }
            else
            {
            */
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
                    renderingContextComponent.mFrustumCulledEntities++;
                    continue;
                }

                RenderEntityInternal
                (
                    entityId,
                    renderableComponent,                    
                    cameraComponent,
                    shaderStoreComponent,
                    windowComponent,                    
                    renderingContextComponent,
                    previousRenderingStateComponent
                );
            }            
        //}
    }
 
    // Sort GUI elements based on z
    std::sort(guiEntities.begin(), guiEntities.end(), [this](const genesis::ecs::EntityId& lhs, const genesis::ecs::EntityId& rhs)
    {
        const auto& lhsTransformComponent = mWorld.GetComponent<TransformComponent>(lhs);
        const auto& rhsTransformComponent = mWorld.GetComponent<TransformComponent>(rhs);
                  
        return lhsTransformComponent.mPosition.z > rhsTransformComponent.mPosition.z;
    });
    
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
            renderingContextComponent,
            previousRenderingStateComponent
        );
    }

    // Swap window buffers
    SDL_GL_SwapWindow(windowComponent.mWindowHandle);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

void RenderingSystem::RenderEntityInternal
(
    const genesis::ecs::EntityId entityId,
    const RenderableComponent& renderableComponent,    
    const CameraSingletonComponent& cameraComponent, 
    const ShaderStoreSingletonComponent& shaderStoreComponent,
    const WindowSingletonComponent& windowComponent,    
    RenderingContextSingletonComponent& renderingContextComponent,
    PreviousRenderingStateSingletonComponent& previousRenderingStateComponent
) const
{
    if (!renderableComponent.mVisibility)
    {
        return;
    }

    // Update Shader is necessary
    const ShaderResource* currentShader = nullptr;
    if (renderableComponent.mShaderNameId != previousRenderingStateComponent.previousShaderNameId)
    {
        currentShader = &shaderStoreComponent.mShaders.at(renderableComponent.mShaderNameId);
        GL_CHECK(glUseProgram(currentShader->GetProgramId()));

        previousRenderingStateComponent.previousShaderNameId = renderableComponent.mShaderNameId;
        previousRenderingStateComponent.previousShader       = currentShader;        
    }
    else
    {
        currentShader = previousRenderingStateComponent.previousShader;
    }

    // Get currently active mesh for this entity
    const auto& transformComponent = mWorld.GetComponent<TransformComponent>(entityId);    
    const auto& activeMeshes       = renderableComponent.mAnimationsToMeshes.at(renderableComponent.mActiveAnimationNameId);

    // Update current mesh if necessary
    const MeshResource* currentMesh = nullptr;
    if (activeMeshes[renderableComponent.mActiveMeshIndex] != previousRenderingStateComponent.previousMeshResourceId)
    {
        currentMesh = &ResourceLoadingService::GetInstance().GetResource<MeshResource>(activeMeshes[renderableComponent.mActiveMeshIndex]);
        GL_CHECK(glBindVertexArray(currentMesh->GetVertexArrayObject()));

        previousRenderingStateComponent.previousMesh           = currentMesh;
        previousRenderingStateComponent.previousMeshResourceId = activeMeshes[renderableComponent.mActiveMeshIndex];
    }
    else
    {
        currentMesh = previousRenderingStateComponent.previousMesh;
    }
        
    // Calculate world matrix for entity
    glm::mat4 world(1.0f);
        
    // Correct display of hud and billboard entities    
    glm::vec3 position = transformComponent.mPosition;
    glm::vec3 scale    = transformComponent.mScale;
    glm::vec3 rotation = transformComponent.mRotation;

    if (!renderableComponent.mAffectedByPerspective)
    {        
        scale.x /= windowComponent.mAspectRatio;        
    }  

    world = glm::translate(world, position + cameraComponent.mGlobalScreenOffset);
    world = glm::rotate(world, rotation.x, math::X_AXIS);
    world = glm::rotate(world, rotation.y, math::Y_AXIS);
    world = glm::rotate(world, rotation.z, math::Z_AXIS);
    world = glm::scale(world, scale);
        
    // Update texture if necessary
    const TextureResource* currentTexture = nullptr;
    if (renderableComponent.mTextureResourceId != previousRenderingStateComponent.previousTextureResourceId)
    {
        currentTexture = &ResourceLoadingService::GetInstance().GetResource<TextureResource>(renderableComponent.mTextureResourceId);
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, currentTexture->GetGLTextureId()));

        previousRenderingStateComponent.previousTexture = currentTexture;
        previousRenderingStateComponent.previousTextureResourceId = renderableComponent.mTextureResourceId;        
    }
    else
    {
        currentTexture = previousRenderingStateComponent.previousTexture;
    }      

    // Set uniforms    
    GL_CHECK(glUniformMatrix4fv(currentShader->GetUniformNamesToLocations().at(WORLD_MARIX_UNIFORM_NAME), 1, GL_FALSE, (GLfloat*)&world));
    GL_CHECK(glUniformMatrix4fv(currentShader->GetUniformNamesToLocations().at(VIEW_MARIX_UNIFORM_NAME), 1, GL_FALSE, (GLfloat*)&cameraComponent.mViewMatrix));
    GL_CHECK(glUniformMatrix4fv(currentShader->GetUniformNamesToLocations().at(PROJECTION_MARIX_UNIFORM_NAME), 1, GL_FALSE, (GLfloat*)&cameraComponent.mProjectionMatrix));    

    renderingContextComponent.mRenderedEntities++;
    
    // Perform draw call
    GL_CHECK(glDrawElements(GL_TRIANGLES, currentMesh->GetElementCount(), GL_UNSIGNED_SHORT, (void*)0));
}

void RenderingSystem::InitializeRenderingWindowAndContext() const
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        ShowMessageBox(MessageBoxType::ERROR, "Error initializing SDL", "An error has occurred while trying to initialize SDL");
        exit(1);
    }

    // Set SDL GL attributes        
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // MS
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    // Get Current Display Mode Resolution
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    
    const auto desiredWindowWidth  = static_cast<int>(displayMode.w * 0.66f);
    const auto desiredWindowHeight = static_cast<int>(desiredWindowWidth/TARGET_ASPECT_RATIO);

    // Create SDL window
    auto windowComponent           = std::make_unique<WindowSingletonComponent>();   
    windowComponent->mWindowTitle  = "Genesis";
    windowComponent->mWindowHandle = SDL_CreateWindow(windowComponent->mWindowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, desiredWindowWidth, desiredWindowHeight, SDL_WINDOW_OPENGL);
   
    if (windowComponent->mWindowHandle == nullptr)
    {
        ShowMessageBox(MessageBoxType::ERROR, "Error creating SDL window", "An error has occurred while trying to create an SDL_Window");
        exit(1);
    }

    // Make window non-resizable and display
    SDL_SetWindowResizable(windowComponent->mWindowHandle, SDL_FALSE);
    SDL_ShowWindow(windowComponent->mWindowHandle);

    // Create SDL GL context
    auto renderingContextComponent = std::make_unique<RenderingContextSingletonComponent>();
    renderingContextComponent->mGLContext = SDL_GL_CreateContext(windowComponent->mWindowHandle);
    if (renderingContextComponent->mGLContext == nullptr)
    {
        ShowMessageBox(MessageBoxType::ERROR, "Error creating SDL context", "An error has occurred while trying to create an SDL_Context");
        exit(1);
    }

    // Commit context 
    SDL_GL_MakeCurrent(windowComponent->mWindowHandle, renderingContextComponent->mGLContext);
    SDL_GL_SetSwapInterval(0);

#ifdef _WIN32
    // Initialize GLES2 function table
    glFuncTable.initialize();
#endif

    // Get actual render buffer width/height
    auto renderableWidth  = 0;
    auto renderableHeight = 0;
    SDL_GL_GetDrawableSize(windowComponent->mWindowHandle, &renderableWidth, &renderableHeight);

    windowComponent->mRenderableWidth  = static_cast<float>(renderableWidth);
    windowComponent->mRenderableHeight = static_cast<float>(renderableHeight);
    windowComponent->mAspectRatio      = windowComponent->mRenderableWidth/windowComponent->mRenderableHeight;
    
    // Log GL driver info
    Log(LogType::INFO, "Vendor     : %s", GL_NO_CHECK(glGetString(GL_VENDOR)));
    Log(LogType::INFO, "Renderer   : %s", GL_NO_CHECK(glGetString(GL_RENDERER)));
    Log(LogType::INFO, "Version    : %s", GL_NO_CHECK(glGetString(GL_VERSION)));   

    // Configure Blending
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    renderingContextComponent->mBlending = true;
    
    // Configure Depth
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(GL_LESS));
    renderingContextComponent->mDepthTest = true;
    
    // Transfer ownership of singleton components to world
    mWorld.SetSingletonComponent<WindowSingletonComponent>(std::move(windowComponent));
    mWorld.SetSingletonComponent<RenderingContextSingletonComponent>(std::move(renderingContextComponent));
    mWorld.SetSingletonComponent<PreviousRenderingStateSingletonComponent>(std::make_unique<PreviousRenderingStateSingletonComponent>());

    // Now that the GL context has been initialized, the ResourceLoadingService
    // can be properly initialized (given that many of them call SDL services)
    ResourceLoadingService::GetInstance().InitializeResourceLoaders();
    genesis::SoundService::GetInstance().InitializeSdlMixer();
}

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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
