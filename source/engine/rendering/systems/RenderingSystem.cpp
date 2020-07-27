///------------------------------------------------------------------------------------------------
///  RenderingSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#include "RenderingSystem.h"
#include "../components/CameraSingletonComponent.h"
#include "../components/LightStoreSingletonComponent.h"
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

namespace rendering
{

///-----------------------------------------------------------------------------------------------

namespace
{
    const StringId WORLD_MARIX_UNIFORM_NAME        = StringId("world");
    const StringId VIEW_MARIX_UNIFORM_NAME         = StringId("view");
    const StringId PROJECTION_MARIX_UNIFORM_NAME   = StringId("proj");
    const StringId NORMAL_MATRIX_UNIFORM_NAME      = StringId("norm");
    const StringId MATERIAL_AMBIENT_UNIFORM_NAME   = StringId("material_ambient");
    const StringId MATERIAL_DIFFUSE_UNIFORM_NAME   = StringId("material_diffuse");
    const StringId MATERIAL_SPECULAR_UNIFORM_NAME  = StringId("material_specular");
    const StringId MATERIAL_SHININESS_UNIFORM_NAME = StringId("material_shininess");
    const StringId ACTIVE_LIGHT_COUNT_UNIFORM_NAME = StringId("active_light_count");
    const StringId LIGHT_POSITIONS_UNIFORM_NAME    = StringId("light_positions");
    const StringId EYE_POSITION_UNIFORM_NAME       = StringId("eye_pos");
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

RenderingSystem::RenderingSystem()
    : BaseSystem()
{
    InitializeRenderingWindowAndContext();
    InitializeCamera();
    InitializeLights();
    CompileAndLoadShaders();
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::VUpdate(const float, const std::vector<ecs::EntityId>& entitiesToProcess) const
{    
    auto& world = ecs::World::GetInstance();

    // Get common rendering singleton components
    const auto& windowComponent      = world.GetSingletonComponent<WindowSingletonComponent>();
    const auto& shaderStoreComponent = world.GetSingletonComponent<ShaderStoreSingletonComponent>();
    const auto& lightStoreComponent  = world.GetSingletonComponent<LightStoreSingletonComponent>();
    auto& cameraComponent            = world.GetSingletonComponent<CameraSingletonComponent>();
    auto& renderingContextComponent  = world.GetSingletonComponent<RenderingContextSingletonComponent>();
    
    // Calculate render-constant camera view matrix
    cameraComponent.mViewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mPosition + cameraComponent.mFrontVector, cameraComponent.mUpVector);
    
    // Calculate render-constant camera projection matrix
    cameraComponent.mProjectionMatrix = glm::perspectiveFovLH
    (
        cameraComponent.mFieldOfView,
        windowComponent.mRenderableWidth,
        windowComponent.mRenderableHeight,
        cameraComponent.mZNear,
        cameraComponent.mZFar
    );


    // Calculate the camera frustum for this frame
    cameraComponent.mFrustum = CalculateCameraFrustum(cameraComponent.mViewMatrix, cameraComponent.mProjectionMatrix);
    
    // Collect all entities that need to be processed
    std::vector<ecs::EntityId> applicableEntities = entitiesToProcess;
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
    std::sort(applicableEntities.begin(), applicableEntities.end(), [&world](const genesis::ecs::EntityId& lhs, const genesis::ecs::EntityId& rhs)
    {
        const auto& lhsTransformComponent = world.GetComponent<TransformComponent>(lhs);
        const auto& rhsTransformComponent = world.GetComponent<TransformComponent>(rhs);

        return lhsTransformComponent.mPosition.z > rhsTransformComponent.mPosition.z;
    });

    for (const auto& entityId : applicableEntities)
    {
        const auto& renderableComponent = world.GetComponent<RenderableComponent>(entityId);
        if (renderableComponent.mIsGuiComponent)
        {
            guiEntities.push_back(entityId);
            continue;
        }
        else
        {
            const auto& transformComponent = world.GetComponent<TransformComponent>(entityId);
            const auto& currentMesh        = resources::ResourceLoadingService::GetInstance().GetResource<resources::MeshResource>(renderableComponent.mMeshResourceId);

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
                transformComponent,
                renderableComponent,
                cameraComponent,
                lightStoreComponent,
                shaderStoreComponent,
                windowComponent,
                renderingContextComponent
            );
        }
    }
    
    // Execute GUI render pass
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    
    for (const auto& entityId : guiEntities)
    {
        const auto& renderableComponent = world.GetComponent<RenderableComponent>(entityId);
        const auto& transformComponent = world.GetComponent<TransformComponent>(entityId);

        RenderEntityInternal
        (            
            transformComponent,
            renderableComponent,            
            cameraComponent,
            lightStoreComponent,
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
    const TransformComponent& transformComponent,
    const RenderableComponent& renderableComponent,    
    const CameraSingletonComponent& cameraComponent,
    const LightStoreSingletonComponent& lightStoreComponent,
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
    const resources::ShaderResource* currentShader = nullptr;
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

    // Update current mesh if necessary
    const resources::MeshResource* currentMesh = nullptr;
    if (renderableComponent.mMeshResourceId != renderingContextComponent.previousMeshResourceId)
    {
        currentMesh = &resources::ResourceLoadingService::GetInstance().GetResource<resources::MeshResource>(renderableComponent.mMeshResourceId);
        GL_CHECK(glBindVertexArray(currentMesh->GetVertexArrayObject()));

        renderingContextComponent.previousMesh           = currentMesh;
        renderingContextComponent.previousMeshResourceId = renderableComponent.mMeshResourceId;
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
    
    glm::mat4 rotMatrix = glm::mat4_cast(math::EulerAnglesToQuat(rotation));
    
    world = glm::translate(world, position);
    world *= rotMatrix;
    world = glm::scale(world, scale);
       

    // Update texture if necessary
    const resources::TextureResource* currentTexture = nullptr;
    if (renderableComponent.mTextureResourceId != renderingContextComponent.previousTextureResourceId)
    {
        currentTexture = &resources::ResourceLoadingService::GetInstance().GetResource<resources::TextureResource>(renderableComponent.mTextureResourceId);
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, currentTexture->GetGLTextureId()));

        renderingContextComponent.previousTexture = currentTexture;
        renderingContextComponent.previousTextureResourceId = renderableComponent.mTextureResourceId;
    }
    else
    {
        currentTexture = renderingContextComponent.previousTexture;
    }      

    // Set mvp uniforms    
    currentShader->SetMatrix4fv(WORLD_MARIX_UNIFORM_NAME, world);
    currentShader->SetMatrix4fv(VIEW_MARIX_UNIFORM_NAME, cameraComponent.mViewMatrix);
    currentShader->SetMatrix4fv(PROJECTION_MARIX_UNIFORM_NAME, cameraComponent.mProjectionMatrix);    
    currentShader->SetMatrix4fv(NORMAL_MATRIX_UNIFORM_NAME, rotMatrix);
    currentShader->SetFloatVec4(MATERIAL_AMBIENT_UNIFORM_NAME, renderableComponent.mMaterial.mAmbient);
    currentShader->SetFloatVec4(MATERIAL_DIFFUSE_UNIFORM_NAME, renderableComponent.mMaterial.mDiffuse);
    currentShader->SetFloatVec4(MATERIAL_SPECULAR_UNIFORM_NAME, renderableComponent.mMaterial.mSpecular);
    currentShader->SetFloat(MATERIAL_SHININESS_UNIFORM_NAME, renderableComponent.mMaterial.mShininess);
    currentShader->SetFloatVec3Array(LIGHT_POSITIONS_UNIFORM_NAME, lightStoreComponent.mLightPositions);
    currentShader->SetInt(ACTIVE_LIGHT_COUNT_UNIFORM_NAME, lightStoreComponent.mLightPositions.size());
    currentShader->SetFloatVec3(EYE_POSITION_UNIFORM_NAME, cameraComponent.mPosition);
    
    // Set other matrix uniforms
    for (const auto& matrixUniformEntry: renderableComponent.mShaderUniforms.mShaderMatrixUniforms)
    {
        currentShader->SetMatrix4fv(matrixUniformEntry.first, matrixUniformEntry.second);
    }

    // Set other float vec4 array uniforms
    for (const auto& vec4arrayUniformEntry: renderableComponent.mShaderUniforms.mShaderFloatVec4ArrayUniforms)
    {
        currentShader->SetFloatVec4Array(vec4arrayUniformEntry.first, vec4arrayUniformEntry.second);
    }
    
    // Set other float vec3 array uniforms
    for (const auto& vec3arrayUniformEntry: renderableComponent.mShaderUniforms.mShaderFloatVec3ArrayUniforms)
    {
        currentShader->SetFloatVec3Array(vec3arrayUniformEntry.first, vec3arrayUniformEntry.second);
    }
    
    // Set other float vec4 uniforms
    for (const auto& floatVec4UniformEntry : renderableComponent.mShaderUniforms.mShaderFloatVec4Uniforms)
    {
        currentShader->SetFloatVec4(floatVec4UniformEntry.first, floatVec4UniformEntry.second);
    }
    
    // Set other float vec3 uniforms
    for (const auto& floatVec3UniformEntry : renderableComponent.mShaderUniforms.mShaderFloatVec3Uniforms)
    {
        currentShader->SetFloatVec3(floatVec3UniformEntry.first, floatVec3UniformEntry.second);
    }
    
    // Set other float uniforms
    for (const auto& floatUniformEntry : renderableComponent.mShaderUniforms.mShaderFloatUniforms)
    {
        currentShader->SetFloat(floatUniformEntry.first, floatUniformEntry.second);
    }
    
    // Set other int uniforms
    for (const auto& intUniformEntry : renderableComponent.mShaderUniforms.mShaderIntUniforms)
    {
        currentShader->SetInt(intUniformEntry.first, intUniformEntry.second);
    }
    
    // Perform draw call
    GL_CHECK(glDrawElements(GL_TRIANGLES, currentMesh->GetElementCount(), GL_UNSIGNED_SHORT, (void*)0));
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::InitializeRenderingWindowAndContext() const
{    
    // Create SDL GL context
    auto renderingContextComponent = std::make_unique<RenderingContextSingletonComponent>();
    auto& windowComponent = ecs::World::GetInstance().GetSingletonComponent<WindowSingletonComponent>();

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
    ecs::World::GetInstance().SetSingletonComponent<RenderingContextSingletonComponent>(std::move(renderingContextComponent));
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::InitializeCamera() const
{        
    ecs::World::GetInstance().SetSingletonComponent<CameraSingletonComponent>(std::make_unique<CameraSingletonComponent>());
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::InitializeLights() const
{
    ecs::World::GetInstance().SetSingletonComponent<LightStoreSingletonComponent>(std::make_unique<LightStoreSingletonComponent>());
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::CompileAndLoadShaders() const
{
    auto& renderingContextComponent = ecs::World::GetInstance().GetSingletonComponent<RenderingContextSingletonComponent>();
    
    // Bind default VAO for correct shader compilation
    GL_CHECK(glGenVertexArrays(1, &renderingContextComponent.mDefaultVertexArrayObject));
    GL_CHECK(glBindVertexArray(renderingContextComponent.mDefaultVertexArrayObject));
    
    const auto shaderNames    = GetAndFilterShaderNames();
    auto shaderStoreComponent = std::make_unique<ShaderStoreSingletonComponent>();
    
    for (const auto& shaderName: shaderNames)
    {
        // By signaling to load either a .vs or a .fs, the ShaderLoader will load the pair automatically,
        // hence why the addition of the .vs here
        auto shaderResourceId = resources::ResourceLoadingService::GetInstance().LoadResource(resources::ResourceLoadingService::RES_SHADERS_ROOT + shaderName + ".vs");
        auto& shaderResource  = resources::ResourceLoadingService::GetInstance().GetResource<resources::ShaderResource>(shaderResourceId);
        
        // Save a copy of the shader to the ShaderStoreComponent
        shaderStoreComponent->mShaders[StringId(shaderName)] = shaderResource;
        
        // And unload the resource
        resources::ResourceLoadingService::GetInstance().UnloadResource(shaderResourceId);
    }
    
    // Unbind any VAO currently bound
    GL_CHECK(glBindVertexArray(0));
    
    ecs::World::GetInstance().SetSingletonComponent<ShaderStoreSingletonComponent>(std::move(shaderStoreComponent));
}

///-----------------------------------------------------------------------------------------------

std::set<std::string> RenderingSystem::GetAndFilterShaderNames() const
{
    const auto vertexAndFragmentShaderFilenames = GetAllFilenamesInDirectory(resources::ResourceLoadingService::RES_SHADERS_ROOT);

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

}

