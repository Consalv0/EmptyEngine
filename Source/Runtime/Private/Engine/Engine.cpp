
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Platform/PlatformDevice.h"
#include "RHI/RHI.h"

#include "Resources/MeshParser.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

namespace EE
{
    bool GWantToTerminate = false;

    bool GameEngine::Initialize()
    {
        GMainApplication = CreateApplication();

        if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK ) != 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to initialize SDL3: {}\n", Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        GDynamicRHI = PlatformCreateDynamicRHI( GMainApplication->GetPreferedRHI() );
        GInput = PlatformCreateInput();
        GPlatformDevice = PlatformCreatePlatformDevice();

        ModelParser::Initialize();

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

    void GameEngine::Run()
    {
        GInput->Initialize();
        GMainApplication->Run();
    }

    void GameEngine::Terminate()
    {
        delete GPlatformDevice;
        delete GInput;
        delete GMainApplication;
        for ( uint64 i = 0; i < windowCount; i++ )
        {
            delete windows[ i ];
        }
        windowCount = 0;

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

        EE_LOG_CORE_INFO( L"Window '{}' destroying!", window->GetName() );
        delete window;
    }

    Window* GameEngine::CreateWindow( const WindowCreateDescription& description )
    {
        Window* window = PlatformCreateWindow( description );

        EE_LOG_CORE_INFO( L"Window '{}' created!", description.name );

        windows.push_back( window );
        windowCount++;
        return window;
    }

    GameEngine* GEngine = NULL;
}