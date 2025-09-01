#pragma once

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
        WindowOption_CustomPosition_Bit = 1 << 4,
    };
    ENUM_FLAGS_OPERATORS( EWindowOptionFlags );

    struct WindowCreateDescription
    {
        U8String name = "Empty Engine";
        EWindowMode windowMode = WindowMode_Windowed;
        EPresentMode presentMode = PresentMode_VSync;
        uint32 width = 0;
        uint32 height = 0;
        int32 positionX = 0;
        int32 positionY = 0;
        float whiteLevel = 200;
        EPixelFormat desiredPixelFormat = PixelFormat_Unknown;
        EColorSpace desiredColorSpace = ColorSpace_Unknown;
        bool compositeAlpha = false;
        EWindowOptionFlags options = WindowOption_None;
    };

    typedef void* WindowHandleRef;

    struct DisplayProperties
    {
        uint32 width = 0;
        uint32 height = 0;
        int32 positionX = 0;
        int32 positionY = 0;
        uint32 usableWidth = 0;
        uint32 usableHeight = 0;
        int32 usablePositionX = 0;
        int32 usablePositionY = 0;
        bool hdrEnabled = false;
    };

    //* Cointains the properties and functions of a window
    class Window
    {
    public:
        typedef std::function<void( const uint32& width, const uint32& height )> OnResizeEvent;
        typedef std::function<void( const int32& x, const int32& y )> OnPositionChangeEvent;
        typedef std::function<void( const EWindowMode& mode )> OnModeChangedEvent;
        typedef std::function<void( bool )> OnHDRChangedEvent;

        struct EventData
        {
            Window* window = NULL;
            OnResizeEvent resizeEvent;
            OnPositionChangeEvent positionChangeEvent;
            OnModeChangedEvent modeChangedEvent;
            OnHDRChangedEvent HDRChangedEvent;
        };

    public:
        Window( const WindowCreateDescription& parameters );

        virtual ~Window();

        //* Set the window display mode
        virtual void SetWindowMode( const EWindowMode& mode );

        //* Initialize window (window creation)
        bool Initialize();

        //* Resize the size of the window
        void Resize( const uint32& width, const uint32& height );

        //* Set position of the window
        void SetPosition( const int32& x, const int32& y );

        //* Rename the window title
        void SetName( const U8String& newName );

        //* Get the window display mode
        virtual const EWindowMode& GetWindowMode() const;

        //* Get the window title name
        virtual const U8String& GetName() const;

        //* Get the width in pixels of the window
        virtual const uint32& GetWidth() const;

        //* Get the height in pixels of the window
        virtual const uint32& GetHeight() const;

        //* Get the X position of the window
        virtual const int32& GetPositionX() const;

        //* Get the Y position of the window
        virtual const int32& GetPositionY() const;

        //* Get present mode
        virtual const EPresentMode& GetPresentMode() const;

        //* Get SDR white level option
        virtual const float& GetWhiteLevel() const;

        //* HDr is available for this window
        virtual const bool& HDREnabled() const;

        //* Get the size of the window in pixels
        virtual void GetSize( uint32& width, uint32& height ) const;

        //* Get the position of the window
        virtual void GetPosition( int32& x, int32& y ) const;

        //* Get the viewport of the window in pixels
        virtual void GetViewport( int& x, int& y, int& width, int& height ) const;

        //* Get the aspect of width divided by height in pixels of the window
        virtual float GetAspectRatio() const;

        //* Makes a window transparent by setting a transparency color.
        virtual bool SetOpacity( const uint8& opacity ) = 0;

        //* Makes the window invisible to input events
        virtual bool SetPassthrough( bool enable ) = 0;

        //* Sets the window icon
        void SetIcon( class PixelMap* icon );

        //* Terminates this window
        void Terminate();

        //* OS specific window handle
        //* Get SDL_Window Pointer
        FORCEINLINE WindowHandleRef GetWindowHandle() const { return _windowHandle; }

        FORCEINLINE bool IsResizable() const { return (_options & WindowOption_Resizable_Bit) > 0; }

        constexpr bool IsUsingCompositeAlpha() const { return _compositeAlpha; }

        constexpr bool GetOpacity() const { return _opacity; }

        constexpr bool IsPassthrough() const { return _passthrough; }

        //* Desired present pixel format
        constexpr const EPixelFormat& GetDesiredPixelFormat() const { return _pixelFormat; };

        //* Desired present color space
        constexpr const EColorSpace& GetDesiredColorSpace() const { return _colorSpace; };

    protected:
        virtual void OnResize( const uint32& width, const uint32& height );
        virtual void OnPositionChange( const int32& x, const int32& y );
        virtual void OnWindowModeChanged( const EWindowMode& mode );
        virtual void OnHDRChanged( bool hdrEnabled );

    public:
        bool closeRequested;

    protected:
        U8String _name;
        EWindowMode _mode;
        WindowHandleRef _windowHandle;
        EWindowOptionFlags _options;
        uint32 _width, _height;
        int32 _positionX, _positionY;
        EPresentMode _presentMode;
        float _whiteLevel;
        bool _hdrEnabled;
        EPixelFormat _pixelFormat;
        EColorSpace _colorSpace;
        EventData _eventData;
        bool _compositeAlpha;
        uint8 _opacity;
        bool _passthrough;
    };

    //* Creates a window
    Window* PlatformCreateWindow( const WindowCreateDescription& description );
}
