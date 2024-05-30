#pragma once

#include "CoreMinimal.h"

#include "Engine/Application.h"
#include "Engine/Window.h"
#include "Engine/Input.h"

#include "Rendering/RenderInterface.h"

namespace EE
{
    class GameEngine
    {
    protected:

        bool initialized = false;

        uint64 frameCount = 0;

        Application* application = NULL;

        RenderInterface* renderingInterface = NULL;

        std::vector<Window*> windows;

        Input* inputManager = NULL;

        AudioDevice* audioDevice = NULL;

        DeviceFunctions* deviceFunctions = NULL;

    public:
        bool Initialize();

        void Run();

        void ShouldTerminate();

        bool WantToTerminate() const;

        void Terminate();

        Window* CreateWindow();

        void DeleteWindow( Window* window );

        //* Get the total count of frames rendered
        FORCEINLINE uint64 GetFrameCount() const { return frameCount; };

        FORCEINLINE Application* GetApplication() const { return application; };

        FORCEINLINE RenderInterface* GetRenderingInterface() const { return renderingInterface; };

        FORCEINLINE Window* GetWindow( int index ) const { return windows[ index ]; };

        FORCEINLINE Window* GetMainWindow() const { return GetWindow( 0 ); };

        FORCEINLINE Input* GetInputManager() const { return inputManager; };

        FORCEINLINE DeviceFunctions* GetPlatformDevice() const { return deviceFunctions; };

        FORCEINLINE AudioDevice* GetAudioDevice() const { return audioDevice; };

    private:
        friend Application;
        friend Window;

        //* Begin of frame functions
        void BeginFrame( Window* window );

        //* End of frame functions
        void EndFrame( Window* window );

        void PollEvents();
    };

    // Global engine pointer.
    extern class GameEngine* GEngine;
}