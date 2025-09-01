
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/Application.h"
#include "Engine/Ticker.h"
#include "Engine/Window.h"
#include "Engine/Input.h"

#include "Math/CoreMath.h"

#if defined(ES_PLATFORM_WINDOWS) & defined(ES_PLATFORM_CUDA)
#include "CUDA/CoreCUDA.h"
#endif

namespace EE
{
    Application::Application() : _initialized( false ), _initializationError( false )
    {
    }

    Application::~Application()
    {
    }

    void Application::Start()
    {
        if ( Initialize() )
        {
            Awake();
        }
    }

    void Application::OnTerminate()
    {
        _initializationError = false;
    }

    bool Application::Initialize()
    {
        if ( _initialized ) return false;

        _initialized = OnInitialize();

        return _initialized;
    }

    void Application::Awake()
    {
        OnAwake();
        Ticker::Tick();
    }

    void Application::MainLoop()
    {
        Ticker::Tick();

        GEngine->PollEvents();
        OnProcessInput();

        OnTick();

        for ( uint64 i = 0; i < GEngine->windowCount; i++ )
        {
            if ( GEngine->windows[ i ]->closeRequested )
            {
                GEngine->DeleteWindow( GEngine->windows[ i ] );
                break;
            }
        }

        GEngine->frameCount++;
    }

    void Application::Terminate()
    {
        GEngine->ShouldTerminate();
        OnTerminate();
    }

    Application* GMainApplication = NULL;

    extern EE::Application* CreateApplication( int argc, U8Char* argv[] );
}