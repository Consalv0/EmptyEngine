
#include "CoreMinimal.h"
#include "Engine/Engine.h"

#include "Resources/MeshParser.h"
#include "Graphics/CPU/CPUGraphicsDevice.h"

#include <SDL3/SDL_events.h>

namespace EE
{
    bool GWantToTerminate = false;

    void GameEngine::Initialize()
    {
        application_ = EE::CreateApplication();
        window_ = Window::Create();
        inputManager_ = Input::Create();
        deviceFunctions_ = DeviceFunctions::Create();
        graphicsDevice_ = new CPUGraphicsDevice();

        ModelParser::Initialize();

        swapChain_ = new SwapChain();
        SwapChainDescription swapChainDesc{};
        swapChainDesc.width = window_->GetWidth();
        swapChainDesc.height = window_->GetHeight();
        swapChainDesc.fullscreen = window_->GetWindowMode();
        swapChainDesc.format = PixelFormat_RGBA32F;
        swapChainDesc.vsync = window_->GetVSync();
        graphicsDevice_->CreateSwapChain( swapChainDesc, window_, *swapChain_);

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
        inputManager_->Initialize();
        application_->Run();
    }

    void GameEngine::BeginFrame()
    {
        graphicsDevice_->RenderPassBegin( *swapChain_, 0 );
    }

    void GameEngine::EndFrame()
    {
        graphicsDevice_->RenderPassEnd( *swapChain_, 0 );
        frameCount_++;
    }

    void GameEngine::Terminate()
    {
        delete swapChain_;
        delete inputManager_;
        delete window_;
        delete application_;
        delete graphicsDevice_;
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