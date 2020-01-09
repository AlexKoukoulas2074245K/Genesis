///------------------------------------------------------------------------------------------------
///  GenesisEngine.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "GenesisEngine.h"
#include "common/components/TransformComponent.h"
#include "common/utils/Logging.h"
#include "common/utils/MathUtils.h"
#include "common/utils/OSMessageBox.h"
#include "debug/components/ConsoleStateSingletonComponent.h"
#include "input/components/InputStateSingletonComponent.h"
#include "input/systems/RawInputHandlingSystem.h"
#include "input/utils/InputUtils.h"
#include "rendering/components/WindowSingletonComponent.h"
#include "rendering/systems/RenderingSystem.h"
#include "rendering/utils/FontUtils.h"
#include "resources/ResourceLoadingService.h"
#include "scripting/service/LuaScriptingService.h"
#include "sound/SoundService.h"

#include <SDL.h> 
#include <SDL_events.h> 
#include <SDL_timer.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace
{
    const StringId CONSOLE_FONT_NAME  = "console_font";
    const int CONSOLE_FONT_ATLAS_COLS = 16;
    const int CONSOLE_FONT_ATLAS_ROWS = 16;
}

///------------------------------------------------------------------------------------------------

static bool AppShouldQuit();

///------------------------------------------------------------------------------------------------
    
GenesisEngine::GenesisEngine()
{            
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::RunGame(const GameStartupParameters& startupParameters, IGame& game)
{
    Initialize(startupParameters);

    game.VOnSystemsInit();
    InitializeDefaultConsoleFont();
    game.VOnGameInit();


    auto dt                = 0.0f;
    auto elapsedTicks      = 0.0f;
    auto dtAccumulator     = 0.0f;
    auto framesAccumulator = 0LL; 

    while (!AppShouldQuit())
    {        
        UpdateFrameStatistics(dt, elapsedTicks, dtAccumulator, framesAccumulator);
        game.VOnUpdate(dt);
        ecs::World::GetInstance().Update(dt);        
    }
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::Initialize(const GameStartupParameters& startupParameters)
{       
    InitializeSdlContextAndWindow(startupParameters);
    InitializeServices();    
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
    auto windowComponent = std::make_unique<rendering::WindowSingletonComponent>();
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

    ecs::World::GetInstance().SetSingletonComponent<rendering::WindowSingletonComponent>(std::move(windowComponent));
}

///-----------------------------------------------------------------------------------------------

void GenesisEngine::InitializeServices() const
{
    resources::ResourceLoadingService::GetInstance().Initialize();
    sound::SoundService::GetInstance().Initialize();
    scripting::LuaScriptingService::GetInstance().Initialize();

    BindDefaultFunctionsToLua();
}

///-----------------------------------------------------------------------------------------------

void GenesisEngine::InitializeDefaultConsoleFont() const
{
    rendering::LoadFont(CONSOLE_FONT_NAME, CONSOLE_FONT_ATLAS_COLS, CONSOLE_FONT_ATLAS_ROWS);
}

///-----------------------------------------------------------------------------------------------

void GenesisEngine::UpdateFrameStatistics(float& dt, float& elapsedTicks, float& dtAccumulator, long long& framesAccumulator) const
{
    // Calculate frame delta
    const auto currentTicks = static_cast<float>(SDL_GetTicks());
    const auto lastFrameTicks = currentTicks - elapsedTicks;

    elapsedTicks = currentTicks;
    dt = lastFrameTicks * 0.001f;

    framesAccumulator++;
    dtAccumulator += dt;

    if (dtAccumulator > 1.0f)
    {
        Log(LogType::INFO, (std::string("FPS: ") + std::to_string(framesAccumulator)).c_str());

        framesAccumulator = 0;
        dtAccumulator = 0.0f;
    }        

    // Make sure the console system has been registered at all
    if (ecs::World::GetInstance().HasSingletonComponent<debug::ConsoleStateSingletonComponent>() == false)
    {
        return;
    }

    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<debug::ConsoleStateSingletonComponent>();

    // Freeze dt in case the console is opened
    if (consoleStateComponent.mEnabled)
    {
        dt = 0.0f;
    }
}

///-----------------------------------------------------------------------------------------------

void GenesisEngine::BindDefaultFunctionsToLua() const
{
    using scripting::LuaScriptingService;

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("CreateEntity", "CreateEntity([optional]entityName) -> entityId", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 0)
        {
            luaScriptingService.LuaPushIntegral(genesis::ecs::World::GetInstance().CreateEntity());
        }
        else if (stackSize == 1)
        {
            const auto entityName = StringId(luaScriptingService.LuaToString(1));
            luaScriptingService.LuaPushIntegral(genesis::ecs::World::GetInstance().CreateEntity(entityName));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 0 or 1) when calling CreateEntity");
        }

        return 1;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("FindEntity", "FindEntity(entityName) -> entityId", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityName = StringId(luaScriptingService.LuaToString(1));
            luaScriptingService.LuaPushIntegral(genesis::ecs::World::GetInstance().FindEntity(entityName));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling FindEntity");
        }

        return 1;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("DestroyEntity", "DestroyEntity(entityId) -> void", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            genesis::ecs::World::GetInstance().DestroyEntity(entityId);
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling DestroyEntity");
        }

        return 0;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("GetEntityPosition", "GetEntityPosition(entityId) -> x,y,z", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 1)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            const auto& transformComponent = genesis::ecs::World::GetInstance().GetComponent<genesis::TransformComponent>(entityId);
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.x));
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.y));
            luaScriptingService.LuaPushDouble(static_cast<double>(transformComponent.mPosition.z));
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 1) when calling GetEntityPosition");
        }

        return 3;
    });

    LuaScriptingService::GetInstance().BindNativeFunctionToLua("SetEntityPosition", "SetEntityPosition(entityId, x, y, z) -> void", [](lua_State*)
    {
        const auto& luaScriptingService = LuaScriptingService::GetInstance();
        const auto stackSize = luaScriptingService.LuaGetIndexOfTopElement();

        if (stackSize == 4)
        {
            const auto entityId = luaScriptingService.LuaToIntegral(1);
            const auto positionX = luaScriptingService.LuaToDouble(2);
            const auto positionY = luaScriptingService.LuaToDouble(3);
            const auto positionZ = luaScriptingService.LuaToDouble(4);

            auto& transformComponent = genesis::ecs::World::GetInstance().GetComponent<genesis::TransformComponent>(entityId);
            transformComponent.mPosition = glm::vec3(positionX, positionY, positionZ);
        }
        else
        {
            luaScriptingService.ReportLuaScriptError("Illegal argument count (expected 4) when calling SetEntityPosition");
        }

        return 0;
    });
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

#ifndef NDEBUG
            case SDL_TEXTINPUT:
            {
                auto& world = ecs::World::GetInstance();
                if (world.HasSingletonComponent<debug::ConsoleStateSingletonComponent>())
                {
                    auto& consoleStateComponent = world.GetSingletonComponent<debug::ConsoleStateSingletonComponent>();
                    if (consoleStateComponent.mEnabled)
                    {
                        consoleStateComponent.mCurrentCommandTextBuffer += std::string(event.text.text);
                    }                    
                }                
            } 
#endif
        }
    }

    return false;
}

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------
