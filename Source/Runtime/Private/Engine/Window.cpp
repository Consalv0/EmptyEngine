
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Ticker.h"
#include "Engine/Window.h"

#include "RHI/RHI.h"
#include "Rendering/PixelMap.h"

#include "Utils/TextFormatting.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>

#include <Utils/VariableWatcher.h>
#include <functional>

namespace EE
{
    bool WindowEventsHandler( void* userData, SDL_Event* sdlEvent )
    {
        Window::EventData* eventData = static_cast<Window::EventData*>( userData );

        if ( sdlEvent->type >= SDL_EVENT_WINDOW_FIRST && sdlEvent->type <= SDL_EVENT_WINDOW_LAST )
        {
            SDL_WindowID windowID = SDL_GetWindowID( (SDL_Window*)eventData->window->GetWindowHandle() );

            if ( sdlEvent->window.windowID != windowID )
                return true;

            switch ( sdlEvent->type )
            {
            case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                eventData->modeChangedEvent( WindowMode_Maximized );
                break;
            case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                eventData->modeChangedEvent( WindowMode_Windowed );
                break;
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            case SDL_EVENT_WINDOW_RESIZED:
            {
                int w, h;
                SDL_GetWindowSizeInPixels( (SDL_Window*)eventData->window->GetWindowHandle(), &w, &h );
                eventData->resizeEvent( w, h );
                break;
            }
            case SDL_EVENT_WINDOW_MOVED:
            {
                int x, y;
                SDL_GetWindowPosition( (SDL_Window*)eventData->window->GetWindowHandle(), &x, &y );
                eventData->positionChangeEvent( x, y );
                break;
            }
            case SDL_EVENT_WINDOW_MAXIMIZED:
            {
                eventData->modeChangedEvent( WindowMode_Maximized );
                break;
            }
            case SDL_EVENT_WINDOW_RESTORED:
            {
                eventData->modeChangedEvent( WindowMode_Windowed );
                break;
            }
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            {
                eventData->window->closeRequested = true;
                break;
            }
            case SDL_EVENT_WINDOW_EXPOSED:
            {
                break;
            }
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
            {
                break;
            }
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            {
                break;
            }
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_FOCUS_LOST:
            {
                break;
            }
            case SDL_EVENT_WINDOW_DESTROYED:
            {
                eventData->window->closeRequested = true;
                break;
            }
            case SDL_EVENT_WINDOW_HDR_STATE_CHANGED:
            {
                eventData->HDRChangedEvent( sdlEvent->display.data1 );
            }
            default:
            {
                EE_LOG_DEBUG( "SDL: Window Event: WindowID({}), Event({})", windowID, sdlEvent->type );
                break;
            }
            }
        }

        return true;
    }

    bool Window::Initialize()
    {
        if ( _windowHandle != NULL ) 
            return false;

        SDL_PropertiesID windowProperties = SDL_CreateProperties();

        uint32 windowFlags = SDL_WINDOW_KEYBOARD_GRABBED;
        EDynamicRHI renderingInterface = EE::GDynamicRHI->GetType();

        if ( renderingInterface == EDynamicRHI::Vulkan )
        {
            windowFlags |= SDL_WINDOW_VULKAN;
        }

        if ( _options & WindowOption_Resizable_Bit )
        {
            windowFlags |= SDL_WINDOW_RESIZABLE;
        }
        if ( _options & WindowOption_Borderless_Bit )
        {
            windowFlags |= SDL_WINDOW_BORDERLESS;
        }
        if ( _options & WindowOption_AlwaysOnTop_Bit )
        {
            windowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
        }
        if ( _options & WindowOption_SkipTaskbar_Bit )
        {
            windowFlags |= SDL_WINDOW_UTILITY;
        }
        if ( _mode == WindowMode_Fullscreen )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }
        if ( _mode == WindowMode_Maximized )
        {
            windowFlags |= SDL_WINDOW_MAXIMIZED;
        }
        if ( _compositeAlpha )
            windowFlags |= SDL_WINDOW_TRANSPARENT;

        SDL_SetStringProperty( windowProperties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, _name.c_str() );
        SDL_SetNumberProperty( windowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, _width );
        SDL_SetNumberProperty( windowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, _height );
        SDL_SetNumberProperty( windowProperties, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, windowFlags );
        
        _windowHandle = SDL_CreateWindowWithProperties( windowProperties );
        SDL_DestroyProperties( windowProperties );
        
        if ( _windowHandle == NULL )
        {
            EE_LOG_CRITICAL( "Window: \"{0}\" could not be initialized: {1}", _name, SDL_GetError() );
            return false;
        }

        SDL_GetWindowPosition( (SDL_Window*)_windowHandle, &_positionX, &_positionY );

        GDynamicRHI->CreateRHIPresentContextOfWindow( this );

        if ( _compositeAlpha )
            SetOpacity( _opacity );

        SDL_SetWindowKeyboardGrab( (SDL_Window*)_windowHandle, false );

        _eventData = Window::EventData
        {
            .window = this,
            .resizeEvent = [this]( const uint32& width, const uint32& height ) constexpr { OnResize( width, height ); },
            .positionChangeEvent = [this]( const int32& x, const int32& y ) constexpr { OnPositionChange( x, y ); },
            .modeChangedEvent = [this]( const EWindowMode& mode ) constexpr { OnWindowModeChanged( mode ); },
            .HDRChangedEvent = [this]( bool hdrEnabled ) constexpr { OnHDRChanged( hdrEnabled ); }
        };

        SDL_AddEventWatch( WindowEventsHandler, (void*)&_eventData );

        return true;
    }

    Window::Window( const WindowCreateDescription& parameters )
    {
        closeRequested = false;
        _windowHandle = NULL;
        _width = parameters.width;
        _height = parameters.height;
        _positionX = parameters.positionX;
        _positionY = parameters.positionY;
        _name = parameters.name;
        _mode = parameters.windowMode;
        _pixelFormat = parameters.desiredPixelFormat;
        _colorSpace = parameters.desiredColorSpace;
        _whiteLevel = parameters.whiteLevel;
        _options = parameters.options;
        _presentMode = parameters.presentMode;
        _compositeAlpha = parameters.compositeAlpha;
        _opacity = 255;
        _passthrough = false;
        
        if ( (_options & WindowOption_CustomPosition_Bit) == 0 )
        {
            _positionX = SDL_WINDOWPOS_CENTERED;
            _positionY = SDL_WINDOWPOS_CENTERED;
        }

        int displayCount;
        SDL_DisplayID* displays = SDL_GetDisplays( &displayCount );

        // get display bounds for all displays
        TMap<SDL_DisplayID, DisplayProperties> displayProperties;
        for ( int i = 0; i < displayCount; i++ )
        {
            DisplayProperties properties{};
            SDL_Rect rect;
            SDL_GetDisplayBounds( displays[ i ], &rect );
            properties.width = rect.w;
            properties.height = rect.h;
            properties.positionX = rect.x;
            properties.positionY = rect.y;
            SDL_GetDisplayUsableBounds( displays[ i ], &rect );
            properties.usableWidth = rect.w;
            properties.usableHeight = rect.h;
            properties.usablePositionX = rect.x;
            properties.usablePositionY = rect.y;

            SDL_PropertiesID displayPropertiesID = SDL_GetDisplayProperties( i );
            properties.hdrEnabled = SDL_GetBooleanProperty( displayPropertiesID, SDL_PROP_DISPLAY_HDR_ENABLED_BOOLEAN, false );

            displayProperties.emplace( displays[ i ], properties );
        }
        SDL_free( displays );

        SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
        if ( (SDL_WINDOWPOS_ISUNDEFINED( _positionX ) || SDL_WINDOWPOS_ISCENTERED( _positionX )) && (_positionX & 0xFFFF) )
        {
            displayID = (_positionX & 0xFFFF);
        }
        else if ( (SDL_WINDOWPOS_ISUNDEFINED( _positionY ) || SDL_WINDOWPOS_ISCENTERED( _positionY )) && (_positionY & 0xFFFF) )
        {
            displayID = (_positionY & 0xFFFF);
        }

        _hdrEnabled = displayProperties[ displayID ].hdrEnabled;
        if ( _width == 0 )
            _width = displayProperties[ displayID ].width;
        if ( _height == 0 )
            _height = displayProperties[ displayID ].height;
    }

    Window::~Window()
    {
        Terminate();
    }

    void Window::Resize( const uint32& width, const uint32& height )
    {
        if ( this->_width != width || this->_height != height )
        {
            SDL_SetWindowSize( (SDL_Window*)(_windowHandle), width, height );
        }
    }

    void Window::SetPosition( const int32& x, const int32& y )
    {
        if ( this->_positionX != x || this->_positionY != y )
        {
            SDL_SetWindowPosition( (SDL_Window*)(_windowHandle), x, y );
        }
    }

    void Window::SetName( const U8String& newName )
    {
        _name = newName;
        SDL_SetWindowTitle( (SDL_Window*)(_windowHandle), _name.c_str() );
    }

    void Window::SetWindowMode( const EWindowMode& mode )
    {
        if ( mode == WindowMode_Fullscreen )
            SDL_SetWindowFullscreen( (SDL_Window*)(_windowHandle), true );
        else if ( mode == WindowMode_Windowed )
        {
            SDL_SetWindowFullscreen( (SDL_Window*)(_windowHandle), false );
            if ( this->IsResizable() )
                SDL_RestoreWindow( (SDL_Window*)(_windowHandle) );
        }
        else if ( mode == WindowMode_Maximized )
        {
            SDL_SetWindowFullscreen( (SDL_Window*)(_windowHandle), false );
            if ( this->IsResizable() )
                SDL_MaximizeWindow( (SDL_Window*)(_windowHandle) );
        }
    }

    const EWindowMode& Window::GetWindowMode() const
    {
        return _mode;
    }

    const float& Window::GetWhiteLevel() const
    {
        return _whiteLevel;
    }

    const bool& Window::HDREnabled() const
    {
        return _hdrEnabled;
    }

    void Window::SetIcon( PixelMap* Icon )
    {
        if ( Icon->GetFormat() != PixelFormat_R8G8B8A8_UINT ) return;
        SDL_Surface* Surface = SDL_CreateSurfaceFrom(
            Icon->GetWidth(), Icon->GetHeight(),
            SDL_PIXELFORMAT_RGBA32,
            (void*)Icon->GetData(),
            NULL
        );
        SDL_SetWindowIcon( static_cast<SDL_Window*>(_windowHandle), Surface );
        SDL_DestroySurface( Surface );
    }

    void Window::Terminate()
    {
#ifdef EE_DEBUG
        EE_LOG_DEBUG( "Window: \"{}\" closed!", GetName() );
#endif // EE_DEBUG

        SDL_RemoveEventWatch( WindowEventsHandler, (void*)this );
        GDynamicRHI->FreeRHIPresentContextOfWindow( this );
        SDL_DestroyWindow( (SDL_Window*)(_windowHandle) );
        _windowHandle = NULL;
    }

    void Window::OnResize( const uint32& width, const uint32& height )
    {
        _width = width; _height = height;
    }

    void Window::OnPositionChange( const int32& x, const int32& y )
    {
        _positionX = x; _positionY = y;
    }

    void Window::OnWindowModeChanged( const EWindowMode& mode )
    {
        _mode = mode;
    }

    void Window::OnHDRChanged( bool hdrEnabled )
    {
        _hdrEnabled = hdrEnabled;
        GDynamicRHI->GetRHIPresentContextOfWindow( this )->SetSwapChainDirty();
    }

    const U8String& Window::GetName() const
    {
        return _name;
    }

    float Window::GetAspectRatio() const
    {
        return (float)_width / (float)_height;
    }

    const uint32& Window::GetWidth() const
    {
        return _width;
    }

    const uint32& Window::GetHeight() const
    {
        return _height;
    }

    const int32& Window::GetPositionX() const
    {
        return _positionX;
    }

    const int32& Window::GetPositionY() const
    {
        return _positionY;
    }

    const EPresentMode& Window::GetPresentMode() const
    {
        return _presentMode;
    }

    void Window::GetSize( uint32& w, uint32& h ) const
    {
        w = _width; h = _height;
    }

    void Window::GetPosition( int32& x, int32& y ) const
    {
        x = _width; y = _height;
    }

    void Window::GetViewport( int& x, int& y, int& w, int& h ) const
    {
        x = 0; y = 0; w = _width; h = _height;
    }
}
