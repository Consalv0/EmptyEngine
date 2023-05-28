#pragma once

#include "CoreMinimal.h"

#include "Engine/Application.h"
#include "Engine/Window.h"
#include "Engine/Input.h"

namespace EE
{
	class GameEngine
	{
	protected:
		
		bool initialized_ = false;

		Application* mainApplication_ = NULL;

		Window* mainWindow_ = NULL;

		Input* inputManager_ = NULL;

		AudioDevice* audioDevice_ = NULL;

		DeviceFunctions* deviceFunctions_ = NULL;

	public:
        void Initialize();

        void Run();

		void ShouldTerminate();

        bool WantToTerminate() const;

		FORCEINLINE Application* GetMainApplication() const { return mainApplication_; };

		FORCEINLINE Window* GetMainWindow() const { return mainWindow_; };

		FORCEINLINE Input* GetInputManager() const { return inputManager_; };

		FORCEINLINE AudioDevice* GetAudioDevice() const { return audioDevice_; };

	private:
        friend Application;

        void PollEvents();

		void Terminate();
	};

    // Global engine pointer.
    extern class GameEngine* GEngine;
}