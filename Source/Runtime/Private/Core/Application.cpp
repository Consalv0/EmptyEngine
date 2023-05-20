
#include "CoreMinimal.h"
#include "Core/Application.h"
#include "Core/CoreTime.h"
#include "Core/Window.h"
#include "Core/Input.h"

#include "Math/CoreMath.h"
#include "Rendering/RenderPipeline.h"

#if defined(ES_PLATFORM_WINDOWS) & defined(ES_PLATFORM_CUDA)
#include "CUDA/CoreCUDA.h"
#endif


namespace EEngine
{
	Application::Application()
	{
		bInitialized = false;
		bRunning = false;
	}

	void Application::Run()
	{
		if ( bRunning ) return;

		bRunning = true;
		LOG_CORE_INFO( L"Initalizing Application:\n" );
		Initalize();
		Awake();
		UpdateLoop();
		Terminate();
	}

	Application* Application::GetInstance()
	{
		static Application* Instance = CreateApplication();
		return Instance;
	}

	RenderPipeline& Application::GetRenderPipeline()
	{
		static RenderPipeline Pipeline;
		return Pipeline;
	}

	void Application::WindowEventCallback( WindowEvent& WinEvent )
	{
		EventDispatcher<WindowEvent> Dispatcher( WinEvent );
		Dispatcher.Dispatch<WindowCloseEvent>( std::bind( &Application::OnWindowClose, this, std::placeholders::_1 ) );
		Dispatcher.Dispatch<WindowResizeEvent>( [this]( WindowResizeEvent& Event )
			{
				GetRenderPipeline().needResize = true;
			} );

		OnWindowEvent( WinEvent );
	}
	
	void Application::InputEventCallback( InputEvent& InEvent )
	{
		OnInputEvent( InEvent );
	}


	void Application::Initalize()
	{
		if ( bInitialized ) return;

		WindowInstance = std::unique_ptr<Window>( Window::Create() );
		WindowInstance->SetWindowEventCallback( std::bind( &Application::WindowEventCallback, this, std::placeholders::_1 ) );
		WindowInstance->SetInputEventCallback( std::bind( &Application::InputEventCallback, this, std::placeholders::_1 ) );

		if ( !GetWindow().IsRunning() ) return;

#ifdef ES_PLATFORM_CUDA
		CUDA::FindCudaDevice();
#endif
		AudioDeviceInstance = std::make_unique<AudioDevice>();
		DeviceFunctionsInstance = std::unique_ptr<DeviceFunctions>( DeviceFunctions::Create() );

		OnInitialize();

		bInitialized = true;
	}

	void Application::Awake()
	{
		if ( !bInitialized ) return;

		OnAwake();
	}

	void Application::UpdateLoop()
	{
		if ( !bInitialized ) return;

		do
		{
			Time::Tick();

			OnUpdate( Time::GetTimeStamp() );

			if ( !Time::bSkipRender )
			{
				GetWindow().BeginFrame();
				GetRenderPipeline().BeginFrame();

				OnRender();
				OnPostRender();

				GetRenderPipeline().EndOfFrame();
				GetWindow().EndFrame();
			}

		} while (
			bRunning == true
			);
	}

	void Application::Terminate()
	{
		ShouldClose();
		OnTerminate();

		DeviceFunctionsInstance.reset( NULL );
		WindowInstance.reset( NULL );
	}

	void Application::OnWindowClose( WindowCloseEvent& CloseEvent )
	{
		Application::ShouldClose();
	}
}