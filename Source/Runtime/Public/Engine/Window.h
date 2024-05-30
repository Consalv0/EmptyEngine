#pragma once

#include "CoreMinimal.h"
#include "Events/WindowEvent.h"
#include "Events/InputEvent.h"
#include "Rendering/Common.h"
#include "Rendering/Objects.h"

union WindowEventData;

namespace EE
{
    enum EWindowMode
    {
        WindowMode_Windowed = 0,
        WindowMode_FullScreen = 1
    };

    enum EWindowOption
    {
        WindowOption_None = 0,
        WindowOption_Borderless = 1 << 0,
        WindowOption_Resizable = 1 << 1,
        WindowOption_AlwaysOnTop = 1 << 2,
        WindowOption_SkipTaskbar = 1 << 3,
    };

    template <typename T>
    class Bitmap;

    struct WindowProperties
    {
        //* Name displayed in header window
        WString name;
        uint32 width;
        uint32 height;
        EWindowMode windowMode;
        bool allowHDR;
        uint32 options;

        WindowProperties(
            const WString& title = L"Empty Engine",
            uint32 width = -1,
            uint32 height = -1,
            EWindowMode mode = WindowMode_Windowed,
            bool allowHDR = false,
            uint32 options = WindowOption_None
        )
            : name(title), width(width), height(height), windowMode(mode), allowHDR(allowHDR), options(options) {
        }

    };

    typedef void* WindowHandleRef;

    //* Cointains the properties and functions of a window
    class Window
    {
    public:
        bool closeRequested;

    protected:
        struct
        {
            WString name;
            EWindowMode mode;
            WindowHandleRef windowHandle;
            PresentContext presentContext;
            uint32 options;
            int32 width;
            int32 height;
            bool allowHDR;
            bool vsync;
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

        //* Get hdr option
        virtual bool GetAllowHDR() const;

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

        //* Graphica present frame context
        const PresentContext& GetPresentContext() const { return presentContext; };

        //* OS specific window handle
        WindowHandleRef GetWindowHandle() const { return windowHandle; };

        //* Creates a Window with a Name, Width and Height
        static Window* Create( const WindowProperties& parameters );
    };

    Window* CreateApplicationWindow();
}
