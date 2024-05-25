
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
        application_ = EE::CreateApplication();
        graphicsDevice_ = EE::CreateGraphicsDevice();
        window_ = EE::CreateWindow();

        if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC ) != 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to initialize SDL3: {0}\n", Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        if ( graphicsDevice_->Initialize() == false )
        {
            return false;
        }
        if ( window_->Initialize() == false )
        {
            return false;
        }


        inputManager_ = Input::Create();
        deviceFunctions_ = DeviceFunctions::Create();

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
        inputManager_->Initialize();
        application_->Run();
    }

    void GameEngine::BeginFrame()
    {
        graphicsDevice_->RenderPassBegin( window_->GetWindowContext().swapChain, 0 );
    }

    void GameEngine::EndFrame()
    {
        graphicsDevice_->RenderPassEnd( window_->GetWindowContext().swapChain, 0 );
        frameCount_++;
    }

    void GameEngine::Terminate()
    {
        delete inputManager_;
        delete window_;
        delete application_;
        delete graphicsDevice_;

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

    GameEngine* GEngine = NULL;
}