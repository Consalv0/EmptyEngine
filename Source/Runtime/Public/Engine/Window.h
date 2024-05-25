#pragma once

#include "CoreMinimal.h"
#include "Events/WindowEvent.h"
#include "Events/InputEvent.h"
#include "Graphics/Graphics.h"

union WindowEventData;

namespace EE
{
    template <typename T>
    class Bitmap;

    struct WindowProperties
    {
        //* Name displayed in header window
        WString name;
        uint32 width;
        uint32 height;
        EWindowMode windowMode;
        uint32 options;

        WindowProperties(
            const WString& title = L"Empty Engine",
            uint32 width = -1,
            uint32 height = -1,
            EWindowMode mode = WindowMode_Windowed,
            uint32 options = WindowOption_None )
            : name(title), width(width), height(height), windowMode(mode), options(options) {
        }

    };

    typedef void* WindowHandle;

    struct WindowContext : GraphicsDeviceObject
    {
        WindowSurface windowSurface;
        SwapChain swapChain;
    };

    //* Cointains the properties and functions of a window
    class Window
    {
    protected:
        struct
        {
            WString name_;
            EWindowMode mode_;
            WindowHandle windowHandle_;
            WindowContext windowContext_;
            uint32 options_;
            int32 width_;
            int32 height_;
            bool vsync_;
        };

    public:
        Window( const WindowProperties& parameters );

        ~Window();

        //* Set the window display mode
        virtual void SetWindowMode(EWindowMode mode);

        //* Initialize window (window creation)
        bool Initialize();

        //* Resize the size of the window
        void Resize( const uint32& width, const uint32& height );

        //* Rename the window title
        void SetName( const WString& newName );

        //* Get the window display mode
        virtual EWindowMode GetWindowMode() const;

        //* Get the window title name
        virtual WString GetName() const;

        //* Get the width in pixels of the window
        virtual int32 GetWidth() const;

        //* Get the height in pixels of the window
        virtual int32 GetHeight() const;

        //* Get vsync options
        virtual bool GetVSync() const;

        //* Get the size of the window in pixels
        virtual IntVector2 GetSize() const;

        //* Get the size of the window in pixels
        virtual IntBox2 GetViewport() const;

        //* Get the aspect of width divided by height in pixels of the window
        virtual float GetAspectRatio() const;

        //* Get SDL_Window Pointer
        virtual void* GetHandle() const;

        //* Makes a window transparent by setting a transparency color.
        virtual bool MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a );

        //* Sets the window icon
        void SetIcon( class PixelMap* Icon );

        //* Terminates this window
        void Terminate();

        //* Graphica device context
        const WindowContext& GetWindowContext() const { return windowContext_; };

        //* OS specific window handle
        WindowHandle GetWindowHandle() const { return windowHandle_; };

        //* Creates a Window with a Name, Width and Height
        static Window* Create( const WindowProperties& parameters );
    };

    Window* CreateWindow();
}
