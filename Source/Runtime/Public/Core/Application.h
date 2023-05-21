#pragma once

#include "Core/Window.h"
#include "Audio/AudioDevice.h"

namespace EEngine
{
	class Application {

	public:
		static Application* GetInstance();

		class RenderPipeline& GetRenderPipeline();

		void WindowEventCallback( WindowEvent& WinEvent );

		void InputEventCallback( InputEvent& InEvent );

		//* Appication Instance
		FORCEINLINE Window& GetWindow() { return *windowInstance_; };

		//* Application Device Functions
		FORCEINLINE DeviceFunctions& GetDeviceFunctions() { return *deviceFunctionsInstance_; };

		//* Application Audio Output
		FORCEINLINE AudioDevice& GetAudioDevice() { return *audioDeviceInstance_; }

		//* Call this to close the application
		FORCEINLINE void ShouldClose() { running_ = false; };

		//* The application is running
		bool IsRunning() { return running_; }

		//* Entry point of the application
		void Run();

		virtual ~Application() = default;

	protected:
		std::unique_ptr<Window> windowInstance_;

		std::unique_ptr<DeviceFunctions> deviceFunctionsInstance_;

		std::unique_ptr<AudioDevice> audioDeviceInstance_;

		Application();

		virtual void OnInitialize() {};

		virtual void OnAwake() {};

		virtual void OnRender() {};

		virtual void OnPostRender() {};

		virtual void OnUpdate( Timestamp stamp ) {};

		virtual void OnWindowEvent( WindowEvent& winEvent ) {};

		virtual void OnInputEvent( InputEvent& inEvent ) {};

		virtual void OnTerminate() {};
	private:
		bool initialized_;

		bool running_;

		//* Initialize the application, it creates a window, a context and loads GL functions.
		void Initalize();

		//* Application loading point, awakens the layers
		void Awake();

		//* Application loop
		void UpdateLoop();

		//* Terminates Application
		void Terminate();

		void OnWindowClose(WindowCloseEvent & CloseEvent);
	};

	Application* CreateApplication();
}