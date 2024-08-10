#pragma once

#include "CoreMinimal.h"

#include "Audio/AudioDevice.h"
#include "Engine/Application.h"
#include "Engine/Window.h"
#include "Engine/Input.h"

namespace EE
{
    class GameEngine
    {
    protected:

        bool initialized = false;

        uint64 frameCount = 0;

        uint64 windowCount = 0;
        TArray<Window*> windows;

        AudioDevice* audioDevice = NULL;

    public:
        bool Initialize( int argc, char** argv );

        void Run();

        void ShouldTerminate();

        bool WantToTerminate() const;

        void Terminate();

        Window* CreateWindow( const WindowCreateDescription& description );

        void DeleteWindow( Window* window );

        //* Get the total count of frames rendered
        FORCEINLINE uint64 GetFrameCount() const { return frameCount; };

        FORCEINLINE const uint64& GetWindowCount() const { return windowCount; };

        FORCEINLINE Window* GetWindow( uint64 index ) const { if ( windowCount > 0 && index < windowCount ) return windows[ index ]; else return NULL; };

        FORCEINLINE Window* GetActiveWindow( uint64 index ) const { return GetWindow( 0 ); };

        FORCEINLINE AudioDevice* GetAudioDevice() const { return audioDevice; };

    private:
        friend Application;
        friend Window;

        void PollEvents();
    };

    // Global engine pointer.
    extern class GameEngine* GEngine;
}