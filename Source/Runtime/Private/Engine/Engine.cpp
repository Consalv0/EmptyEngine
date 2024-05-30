
#include "CoreMinimal.h"
#include "Engine/Engine.h"

#include "Resources/MeshParser.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

namespace EE
{
    bool GWantToTerminate = false;

    bool GameEngine::Initialize()
    {
        application = EE::CreateApplication();
        renderingInterface = EE::CreateRenderingInterface();
        CreateWindow();

        if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK ) != 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to initialize SDL3: {0}\n", Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        if ( windows.size() > 0 )
        {
            if ( renderingInterface->Initialize() == false )
            {
                return false;
            }
            if ( GetMainWindow()->Initialize() == false )
            {
                return false;
            }
        }

        inputManager = Input::Create();
        deviceFunctions = DeviceFunctions::Create();

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
        inputManager->Initialize();
        application->Run();
    }

    void GameEngine::BeginFrame( Window* window )
    {
        renderingInterface->RenderPassBegin( window->GetPresentContext().swapChain, 0 );
    }

    void GameEngine::EndFrame( Window* window )
    {
        renderingInterface->RenderPassEnd( window->GetPresentContext().swapChain, 0 );
        frameCount++;
    }

    void GameEngine::Terminate()
    {
        delete inputManager;
        size_t windowCount = windows.size();
        for ( size_t i = 0; i < windowCount; i++ )
        {
            delete windows[ i ];
        }
        delete application;
        delete renderingInterface;

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
        delete window;

        if ( windows.size() == 0 )
            ShouldTerminate();
    }

    Window* GameEngine::CreateWindow()
    {
        Window* window = EE::CreateApplicationWindow();
        windows.push_back( window );
        return window;
    }

    GameEngine* GEngine = NULL;
}