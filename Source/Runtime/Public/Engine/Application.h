#pragma once

#include "Audio/AudioDevice.h"

namespace EE
{
	class Application {

	public:
		class RenderPipeline& GetRenderPipeline();

		//* Entry point of the application
		void Run();

		virtual ~Application() = default;

	protected:

		Application();

		virtual void OnInitialize() {};

		virtual void OnAwake() {};

		virtual void OnRender() {};

		virtual void OnPostRender() {};

		virtual void OnUpdate( Timestamp stamp ) {};

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
	};

	Application* CreateApplication();
}