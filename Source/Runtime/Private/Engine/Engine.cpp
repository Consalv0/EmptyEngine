
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Input.h"
#include "RHI/RHI.h"
#include "Physics/PhysicsEngine.h"

#include "Platform/PlatformDevice.h"

#include "Utils/TextFormatting.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

namespace EE
{
    bool GWantToTerminate = false;

    bool GameEngine::Initialize( int argc, U8Char* argv[] )
    {
        GMainApplication = CreateApplication( argc, argv );
        if ( GMainApplication->HasErrors() == false )
        {
            EE_LOG_CRITICAL( "Failed to initialize application!\n" );
            return false;
        }

        if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK ) == false )
        {
            EE_LOG_CRITICAL( "Failed to initialize SDL3: {}\n", SDL_GetError() );
            return false;
        }

        GDynamicRHI = PlatformCreateDynamicRHI( GMainApplication->GetPreferedRHI() );
        GInput = PlatformCreateInput();
        GPlatformDevice = PlatformCreatePlatformDevice();
        GPhysicsEngine = CreatePhysicsEngine();

#ifdef EE_PLATFORM_CUDA
        CUDA::FindCudaDevice();
#endif
        return true;
    }

    void GameEngine::PollEvents()
    {
        SDL_Event sdlEvent;
        while ( SDL_PollEvent( &sdlEvent ) ) {}
    }

    void GameEngine::Start()
    {
        GInput->Initialize();
        GMainApplication->Start();
    }

    void GameEngine::MainLoop()
    {
        GMainApplication->MainLoop();
    }

    void GameEngine::Terminate()
    {
        for ( uint64 i = 0; i < windowCount; i++ )
        {
            delete windows[ i ];
        }
        windowCount = 0;

        delete GPlatformDevice;
        delete GInput;
        delete GMainApplication;
        delete GPhysicsEngine;

        SDL_Quit();
    }

    void GameEngine::ShouldTerminate()
    {
        GWantToTerminate = true;
    }

    bool GameEngine::WantToTerminate() const
    {
        return GWantToTerminate;
    }

    void GameEngine::DeleteWindow( Window* window )
    {
        windows.erase( std::remove( windows.begin(), windows.end(), window ) );
        windowCount--;

        delete window;
    }

    Window* GameEngine::CreateWindow( const WindowCreateDescription& description )
    {
        Window* window = PlatformCreateWindow( description );

        windows.push_back( window );
        windowCount++;
        return window;
    }

    GameEngine* GEngine = NULL;
}