///------------------------------------------------------------------------------------------------
///  GenesisEngine.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "GenesisEngine.h"
#include "input/components/InputStateSingletonComponent.h"
#include "input/systems/RawInputHandlingSystem.h"
#include "input/utils/InputUtils.h"
#include "rendering/systems/RenderingSystem.h"

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
    Initialize();
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::RunGame(IGame& game)
{
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

void GenesisEngine::Initialize()
{    
    auto renderingSystem = std::make_unique<RenderingSystem>(mWorld);
    mWorld.AddSystem(std::make_unique<RawInputHandlingSystem>(mWorld));
    mWorld.AddSystem(std::move(renderingSystem));    
}

///------------------------------------------------------------------------------------------------

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
