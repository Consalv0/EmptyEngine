#pragma once

#include "CoreMinimal.h"

#include "Events/WindowEvent.h"
#include "Events/InputEvent.h"
#include "Rendering/Common.h"

union WindowEventData;

namespace EE
{
    enum EWindowMode
    {
        WindowMode_Windowed = 0,
        WindowMode_Fullscreen = 1,
        WindowMode_Maximized = 2,
    };

    enum EWindowOptionFlags : uint32
    {
        WindowOption_None = 0,
        WindowOption_Borderless_Bit =  1 << 0,
        WindowOption_Resizable_Bit =   1 << 1,
        WindowOption_AlwaysOnTop_Bit = 1 << 2,
        WindowOption_SkipTaskbar_Bit = 1 << 3,
    };
    ENUM_FLAGS_OPERATORS( EWindowOptionFlags );

    struct WindowCreateDescription
    {
        WString name = L"Empty Engine";
        //* Name displayed in header window
        EWindowMode windowMode = WindowMode_Windowed;
        EPresentMode presentMode = PresentMode_VSync;
        uint32 width = 0;
        uint32 height = 0;
        float whiteLevel = 200;
        bool allowHDR = false;
        EWindowOptionFlags options = WindowOption_Resizable_Bit;
    };

    typedef void* WindowHandleRef;

    //* Cointains the properties and functions of a window
    class Window
    {
    public:
        typedef std::function<void( const uint32& width, const uint32& height )> OnResizeEvent;
        typedef std::function<void( const EWindowMode& mode )> OnModeChangedEvent;

        struct EventData
        {
            Window* window;
            OnResizeEvent resizeEvent;
            OnModeChangedEvent modeChangedEvent;
        };

    public:
        Window( const WindowCreateDescription& parameters );

        ~Window();

        //* Set the window display mode
        virtual void SetWindowMode( const EWindowMode& mode );

        //* Initialize window (window creation)
        bool Initialize();

        //* Resize the size of the window
        void Resize( const uint32& width, const uint32& height );

        //* Rename the window title
        void SetName( const WString& newName );

        //* Get the window display mode
        virtual const EWindowMode& GetWindowMode() const;

        //* Get the window title name
        virtual const WString& GetName() const;

        //* Get the width in pixels of the window
        virtual const uint32& GetWidth() const;

        //* Get the height in pixels of the window
        virtual const uint32& GetHeight() const;

        //* Get present mode
        virtual const EPresentMode& GetPresentMode() const;

        //* Get SDR white level option
        virtual const float& GetWhiteLevel() const;

        //* Get hdr option
        virtual const bool& GetAllowHDR() const;

        //* Get the size of the window in pixels
        virtual void GetSize( uint32& width, uint32& height ) const;

        //* Get the viewport of the window in pixels
        virtual void GetViewport( int& x, int& y, int& width, int& height ) const;

        //* Get the aspect of width divided by height in pixels of the window
        virtual float GetAspectRatio() const;

        //* Makes a window transparent by setting a transparency color.
        virtual bool MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a );

        //* Sets the window icon
        void SetIcon( class PixelMap* icon );

        //* Terminates this window
        void Terminate();

        //* OS specific window handle
        //* Get SDL_Window Pointer
        FORCEINLINE WindowHandleRef GetWindowHandle() const { return windowHandle_; };

        FORCEINLINE bool IsResizable() const { return (options_ & WindowOption_Resizable_Bit) > 0; };

    protected:
        virtual void OnResize( const uint32& width, const uint32& height );

        virtual void OnWindowModeChanged( const EWindowMode& mode );

    public:
        bool closeRequested;

    protected:
        WString name_;
        EWindowMode mode_;
        WindowHandleRef windowHandle_;
        EWindowOptionFlags options_;
        uint32 width_, height_;
        EPresentMode presentMode_;
        float whiteLevel_;
        bool allowHDR_;
        uint32 windowedWidth_, windowedHeight_;
        EventData eventData_;
    };

    //* Creates a window
    Window* PlatformCreateWindow( const WindowCreateDescription& description );
}
