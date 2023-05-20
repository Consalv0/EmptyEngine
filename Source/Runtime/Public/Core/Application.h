#pragma once

#include "Core/Window.h"
#include "Audio/AudioDevice.h"

namespace EEngine
{
	class Application {

	public:
		static Application * GetInstance();

		class RenderPipeline & GetRenderPipeline();

		void WindowEventCallback( WindowEvent& WinEvent );

		void InputEventCallback( InputEvent& InEvent );

		//* Appication Instance
		inline Window & GetWindow() { return *WindowInstance; };

		//* Application Device Functions
		inline DeviceFunctions & GetDeviceFunctions() { return *DeviceFunctionsInstance; };

		//* Application Audio Output
		inline AudioDevice & GetAudioDevice() { return *AudioDeviceInstance; }

		//* Call this to close the application
		inline void ShouldClose() { bRunning = false; };

		//* The application is running
		bool IsRunning() { return bRunning; }

		//* Entry point of the application
		void Run();

		virtual ~Application() = default;

	protected:
		std::unique_ptr<Window> WindowInstance;

		std::unique_ptr<DeviceFunctions> DeviceFunctionsInstance;

		std::unique_ptr<AudioDevice> AudioDeviceInstance;

		Application();

		virtual void OnInitialize() {};

		virtual void OnAwake() {};

		virtual void OnRender() {};

		virtual void OnPostRender() {};

		virtual void OnUpdate( Timestamp Stamp ) {};

		virtual void OnWindowEvent( WindowEvent& WinEvent ) {};

		virtual void OnInputEvent( InputEvent& InEvent ) {};

		virtual void OnTerminate() {};
	private:
		bool bInitialized;

		bool bRunning;

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

	Application * CreateApplication();

}