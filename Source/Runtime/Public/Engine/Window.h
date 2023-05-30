#pragma once

#include "CoreMinimal.h"
#include "Events/WindowEvent.h"
#include "Events/InputEvent.h"
#include "Graphics/Graphics.h"
#include "Graphics/GraphicsDevice.h"

union WindowEventData;

namespace EE
{
    enum EWindowMode
    {
        WindowMode_Windowed = 0,
        WindowMode_FullScreen = 1
    };

    template <typename T>
    class Bitmap;

    struct WindowProperties
    {
        //* Name displayed in header window
        WString name;
        uint32_t width;
        uint32_t height;
        EWindowMode windowMode;

        WindowProperties(
            const WString& title = L"Empty Engine",
            uint32_t width = 1280,
            uint32_t height = 720,
            EWindowMode mode = WindowMode_Windowed )
            : name(title), width(width), height(height), windowMode(mode) {
        }

    };

    //* Cointains the properties and functions of a window
    class Window
    {
    protected:
        struct
        {
            WString name_;
            EWindowMode mode_;
            void* windowHandle_;
            int width_;
            int height_;
            bool vsync_;
        };

        void Initialize();

    public:
        Window( const WindowProperties& parameters );

        ~Window();

        //* Set the window display mode
        virtual void SetWindowMode(EWindowMode mode);

        //* Resize the size of the window
        void Resize( const uint32_t& width, const uint32_t& height );

        //* Rename the window title
        void SetName( const WString& newName );

        //* Get the window display mode
        virtual EWindowMode GetWindowMode() const;

        //* Get the window title name
        virtual WString GetName() const;

        //* Get the width in pixels of the window
        virtual int GetWidth() const;

        //* Get the height in pixels of the window
        virtual int GetHeight() const;

        //* Get vsync options
        virtual bool GetVSync() const;

        //* Get the size of the window in pixels
        virtual IntVector2 GetSize() const;

        //* Get the size of the window in pixels
        virtual IntBox2D GetViewport() const;

        //* Get the aspect of width divided by height in pixels of the window
        virtual float GetAspectRatio() const;

        //* Get SDL_Window Pointer
        virtual void* GetHandle() const;

        //* Sets the window icon
        void SetIcon( class PixelMap* Icon );

        //* Terminates this window
        void Terminate();

        //* Creates a Window with a Name, Width and Height
        static Window* Create(const WindowProperties& parameters = WindowProperties());
    };

}
