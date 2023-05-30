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

        uint64_t frameCount_ = 0;

        SwapChain* swapChain_;

        Application* application_ = NULL;

        GraphicsDevice* graphicsDevice_ = NULL;

		Window* window_ = NULL;

		Input* inputManager_ = NULL;

		AudioDevice* audioDevice_ = NULL;

		DeviceFunctions* deviceFunctions_ = NULL;

	public:
        void Initialize();

        void Run();

		void ShouldTerminate();

        bool WantToTerminate() const;

        void Terminate();

        //* Get the total count of frames rendered
        FORCEINLINE uint64_t GetFrameCount() const { return frameCount_; };

		FORCEINLINE Application* GetApplication() const { return application_; };

        FORCEINLINE GraphicsDevice* GetGraphicsDevice() const { return graphicsDevice_; };

        FORCEINLINE Window* GetWindow() const { return window_; };

        FORCEINLINE SwapChain* GetSwapChain() const { return swapChain_; };

		FORCEINLINE Input* GetInputManager() const { return inputManager_; };

		FORCEINLINE AudioDevice* GetAudioDevice() const { return audioDevice_; };

	private:
        friend Application;
        friend Window;

        //* Begin of frame functions
        void BeginFrame();

        //* End of frame functions
        void EndFrame();

        void PollEvents();
	};

    // Global engine pointer.
    extern class GameEngine* GEngine;
}