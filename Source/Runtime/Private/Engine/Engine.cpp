
#include "CoreMinimal.h"
#include "Engine/Engine.h"

#include <SDL3/SDL_events.h>

namespace EE
{
    bool GWantToTerminate = false;

    void GameEngine::Initialize()
    {
        mainApplication_ = EE::CreateApplication();
        mainWindow_ = Window::Create();
        inputManager_ = Input::Create();
        deviceFunctions_ = DeviceFunctions::Create();

#ifdef EE_PLATFORM_CUDA
        CUDA::FindCudaDevice();
#endif
    }

    void GameEngine::PollEvents()
    {
        SDL_Event sdlEvent;
        while ( SDL_PollEvent( &sdlEvent ) ) {}
    }

    void GameEngine::Run()
    {
        inputManager_->CheckForConnectedJoysticks();
        mainApplication_->Run();
    }

    void GameEngine::Terminate()
    {
        delete inputManager_;
        delete mainWindow_;
        delete mainApplication_;
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