﻿
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
    Application::Application()
    {
        initialized_ = false;
    }

    Application::~Application()
    {
        Terminate();
    }

    void Application::Run()
    {
        EE_LOG_CORE_INFO( L"Initalizing Application:\n" );
        Initalize();
        Awake();
        UpdateLoop();
    }

    RenderPipeline& Application::GetRenderPipeline()
    {
        static RenderPipeline* pipeline;
        return *pipeline;
    }

    void Application::Initalize()
    {
        if ( initialized_ ) return;

        OnInitialize();

        initialized_ = true;
    }

    void Application::Awake()
    {
        if ( !initialized_ ) return;

        OnAwake();
    }

    void Application::UpdateLoop()
    {
        if ( !initialized_ ) return;

        do
        {
            Ticker::Tick();
            GEngine->PollEvents();

            GEngine->GetInputManager()->Update();
            OnUpdate( Ticker::GetTimeStamp() );

            if ( !Ticker::IsSkippingRender() )
            {
                size_t windowCount = GEngine->windows.size();
                for ( size_t i = 0; i < windowCount; i++ )
                {
                    GEngine->BeginFrame( GEngine->windows[ i ] );
                    // GetRenderPipeline().BeginFrame();

                    OnRender();
                    OnPostRender();

                    // GetRenderPipeline().EndOfFrame();
                    GEngine->EndFrame( GEngine->windows[ i ] );
                }
            }

        } while (
            GEngine->WantToTerminate() == false
        );
    }

    void Application::Terminate()
    {
        OnTerminate();
    }
}