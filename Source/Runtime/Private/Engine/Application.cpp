
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/Application.h"
#include "Engine/Ticker.h"
#include "Engine/Window.h"
#include "Engine/Input.h"

#include "Math/CoreMath.h"
#include "Rendering/RenderPipeline.h"

#if defined(ES_PLATFORM_WINDOWS) & defined(ES_PLATFORM_CUDA)
#include "CUDA/CoreCUDA.h"
#endif

namespace EE
{
    Application::Application() : initialized( false ), canInitialize( false )
    {
    }

    Application::~Application()
    {
        OnDestroy();
    }

    void Application::Run()
    {
        Initialize();
        Awake();
        UpdateLoop();
    }

    RenderPipeline& Application::GetRenderPipeline()
    {
        static RenderPipeline* pipeline;
        return *pipeline;
    }

    void Application::OnTerminate()
    {
        canInitialize = false;
    }

    bool Application::Initialize()
    {
        if ( initialized ) return false;

        initialized = OnInitialize();

        return initialized;
    }

    void Application::Awake()
    {
        if ( !initialized ) return;

        OnAwake();
    }

    void Application::UpdateLoop()
    {
        if ( !initialized ) return;

        do
        {
            Ticker::Tick();
            GEngine->PollEvents();

            OnUpdate( Ticker::GetTimeStamp() );

            for ( uint64 i = 0; i < GEngine->windowCount; i++ )
            {
                if ( GEngine->windows[ i ]->closeRequested )
                {
                    GEngine->DeleteWindow( GEngine->windows[ i ] );
                    break;
                }
            }

            GEngine->frameCount++;

        } while (
            GEngine->WantToTerminate() == false
        );
    }

    void Application::Terminate()
    {
        GEngine->ShouldTerminate();
        OnTerminate();
    }

    Application* GMainApplication = NULL;

    extern EE::Application* EE::CreateApplication( int argc, char** argv );
}