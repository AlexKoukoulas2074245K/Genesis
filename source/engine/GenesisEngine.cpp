///------------------------------------------------------------------------------------------------
///  GenesisEngine.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "GenesisEngine.h"
#include "common/utils/OSMessageBox.h"
#include "input/components/InputStateSingletonComponent.h"
#include "input/systems/RawInputHandlingSystem.h"
#include "input/utils/InputUtils.h"
#include "rendering/components/WindowSingletonComponent.h"
#include "rendering/systems/RenderingSystem.h"
#include "resources/ResourceLoadingService.h"
#include "sound/SoundService.h"

#include <SDL.h> 
#include <SDL_events.h> 
#include <SDL_timer.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

bool AppShouldQuit();

///------------------------------------------------------------------------------------------------

GenesisEngine::GenesisEngine()
{    
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::RunGame(const GameStartupParameters& startupParameters, IGame& game)
{
    Initialize(startupParameters);

    game.VOnInit(mWorld);

    float elapsedTicks          = 0.0f;
    float dtAccumulator         = 0.0f;
    long long framesAccumulator = 0; 

    while (!AppShouldQuit())
    {
        // Calculate frame delta
        const auto currentTicks   = static_cast<float>(SDL_GetTicks());
        const auto lastFrameTicks = currentTicks - elapsedTicks;

        elapsedTicks  = currentTicks;
        const auto dt = lastFrameTicks * 0.001f;

        framesAccumulator++;
        dtAccumulator += dt;

        if (dtAccumulator > 1.0f)
        {            
            //const auto fpsString = " - FPS: " + std::to_string(framesAccumulator);            
            //const auto entityCountString = " - Entities: " + std::to_string(mWorld.GetActiveEntities().size());            
            //SDL_SetWindowTitle(windowComponent.mWindowHandle, (windowComponent.mWindowTitle + fpsString + entityCountString).c_str());

            framesAccumulator = 0;
            dtAccumulator = 0.0f;
        }

        game.VOnUpdate(dt, mWorld);

        mWorld.Update(dt);
    }
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::Initialize(const GameStartupParameters& startupParameters)
{       
    InitializeSdlContextAndWindow(startupParameters);
    InitializeServices();
    InitializeSystems();
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::InitializeSdlContextAndWindow(const GameStartupParameters& startupParameters)
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
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    const auto screenAspectRatio = displayMode.w / static_cast<float>(displayMode.h);

    auto requestedGameWidth = startupParameters.mGameWindowWidth;
    auto requestedGameHeight = startupParameters.mGameWindowHeight;

    // Check whether fraction constructor used
    if (requestedGameWidth == 0)
    {
        requestedGameWidth  = static_cast<int>(static_cast<float>(displayMode.w) * startupParameters.mGameWindowScreenFraction);
        requestedGameHeight = static_cast<int>(requestedGameWidth / screenAspectRatio);
    }
    
    // Create SDL window
    auto windowComponent = std::make_unique<WindowSingletonComponent>();
    windowComponent->mWindowTitle = startupParameters.mGameTitle;
    windowComponent->mWindowHandle = SDL_CreateWindow
    (
        windowComponent->mWindowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        requestedGameWidth,
        requestedGameHeight,
        SDL_WINDOW_OPENGL
    );

    if (windowComponent->mWindowHandle == nullptr)
    {
        ShowMessageBox(MessageBoxType::ERROR, "Error creating SDL window", "An error has occurred while trying to create an SDL_Window");
        exit(1);
    }

    // Make window non-resizable and display
    SDL_SetWindowResizable(windowComponent->mWindowHandle, SDL_FALSE);
    SDL_ShowWindow(windowComponent->mWindowHandle);

    mWorld.SetSingletonComponent<WindowSingletonComponent>(std::move(windowComponent));
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::InitializeSystems()
{    
    mWorld.AddSystem(std::make_unique<input::RawInputHandlingSystem>(mWorld));
    mWorld.AddSystem(std::make_unique<RenderingSystem>(mWorld));
}

///-----------------------------------------------------------------------------------------------

void GenesisEngine::InitializeServices() const
{
    res::ResourceLoadingService::GetInstance().Initialize();
    sound::SoundService::GetInstance().Initialize();
}

///-----------------------------------------------------------------------------------------------

bool AppShouldQuit()
{
    // Poll events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT: return true;
        }
    }

    return false;
}

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------
