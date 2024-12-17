
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
            default:
            {
                EE_LOG_DEBUG( L"SDL: Window Event: WindowID({}), Event({})", windowID, sdlEvent->type );
                break;
            }
            }
        }

        return true;
    }

    bool Window::Initialize()
    {
        if ( windowHandle_ != NULL ) 
            return false;

        SDL_PropertiesID windowProperties = SDL_CreateProperties();

        uint32 windowFlags = SDL_WINDOW_KEYBOARD_GRABBED;
        EDynamicRHI renderingInterface = EE::GDynamicRHI->GetType();

        if ( renderingInterface == EDynamicRHI::Vulkan )
        {
            windowFlags |= SDL_WINDOW_VULKAN;
        }

        if ( options_ & WindowOption_Resizable_Bit )
        {
            windowFlags |= SDL_WINDOW_RESIZABLE;
        }
        if ( options_ & WindowOption_Borderless_Bit )
        {
            windowFlags |= SDL_WINDOW_BORDERLESS;
        }
        if ( options_ & WindowOption_AlwaysOnTop_Bit )
        {
            windowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
        }
        if ( options_ & WindowOption_SkipTaskbar_Bit )
        {
            windowFlags |= SDL_WINDOW_UTILITY;
        }
        if ( mode_ == WindowMode_Fullscreen )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }
        if ( mode_ == WindowMode_Maximized )
        {
            windowFlags |= SDL_WINDOW_MAXIMIZED;
        }
        if ( compositeAlpha_ )
            windowFlags |= SDL_WINDOW_TRANSPARENT;

        SDL_SetStringProperty( windowProperties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, Text::WideToNarrow( name_ ).c_str() );
        SDL_SetNumberProperty( windowProperties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width_ );
        SDL_SetNumberProperty( windowProperties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height_ );
        SDL_SetNumberProperty( windowProperties, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, windowFlags );
        
        windowHandle_ = SDL_CreateWindowWithProperties( windowProperties );
        SDL_DestroyProperties( windowProperties );
        
        if ( windowHandle_ == NULL )
        {
            EE_LOG_CRITICAL( L"Window: \"{0}\" could not be initialized: {1}", name_, Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        SDL_GetWindowPosition( (SDL_Window*)windowHandle_, &positionX_, &positionY_ );

        GDynamicRHI->CreateRHIPresentContextOfWindow( this );

        if ( compositeAlpha_ )
            SetOpacity( opacity_ );

        SDL_SetWindowKeyboardGrab( (SDL_Window*)windowHandle_, false );

        eventData_ = Window::EventData
        {
            .window = this,
            .resizeEvent = [this]( const uint32& width, const uint32& height ) constexpr { OnResize( width, height ); },
            .positionChangeEvent = [this]( const int32& x, const int32& y ) constexpr { OnPositionChange( x, y ); },
            .modeChangedEvent = [this]( const EWindowMode& mode ) constexpr { OnWindowModeChanged( mode ); }
        };

        SDL_AddEventWatch( WindowEventsHandler, (void*)&eventData_ );

        return true;
    }

    Window::Window( const WindowCreateDescription& parameters )
    {
        closeRequested = false;
        windowHandle_ = NULL;
        width_ = parameters.width;
        height_ = parameters.height;
        positionX_ = parameters.positionX;
        positionY_ = parameters.positionY;
        name_ = parameters.name;
        mode_ = parameters.windowMode;
        allowHDR_ = parameters.allowHDR;
        whiteLevel_ = parameters.whiteLevel;
        options_ = parameters.options;
        presentMode_ = parameters.presentMode;
        compositeAlpha_ = parameters.compositeAlpha;
        opacity_ = 255;
        passthrough_ = false;
        
        if ( (options_ & WindowOption_CustomPosition_Bit) == 0 )
        {
            positionX_ = SDL_WINDOWPOS_CENTERED;
            positionY_ = SDL_WINDOWPOS_CENTERED;
        }

        int displayCount;
        SDL_DisplayID* displays = SDL_GetDisplays( &displayCount );

        // get display bounds for all displays
        TMap<SDL_DisplayID, SDL_Rect> displayBounds;
        for ( int i = 0; i < displayCount; i++ )
        {
            SDL_Rect rect;
            SDL_GetDisplayUsableBounds( displays[ i ], &rect );
            displayBounds.emplace( displays[ i ], rect );
        }
        SDL_free( displays );

        SDL_DisplayID displayID = SDL_GetPrimaryDisplay();
        if ( (SDL_WINDOWPOS_ISUNDEFINED( positionX_ ) || SDL_WINDOWPOS_ISCENTERED( positionX_ )) && (positionX_ & 0xFFFF) )
        {
            displayID = (positionX_ & 0xFFFF);
        }
        else if ( (SDL_WINDOWPOS_ISUNDEFINED( positionY_ ) || SDL_WINDOWPOS_ISCENTERED( positionY_ )) && (positionY_ & 0xFFFF) )
        {
            displayID = (positionY_ & 0xFFFF);
        }

        if ( width_ == 0 )
            width_ = displayBounds[ displayID ].w;
        if ( height_ == 0 )
            height_ = displayBounds[ displayID ].h;
    }

    Window::~Window()
    {
        Terminate();
    }

    void Window::Resize( const uint32& width, const uint32& height )
    {
        if ( this->width_ != width || this->height_ != height )
        {
            SDL_SetWindowSize( (SDL_Window*)(windowHandle_), width, height );
        }
    }

    void Window::SetPosition( const int32& x, const int32& y )
    {
        if ( this->positionX_ != x || this->positionY_ != y )
        {
            SDL_SetWindowPosition( (SDL_Window*)(windowHandle_), x, y );
        }
    }

    void Window::SetName( const WString& newName )
    {
        name_ = newName;
        SDL_SetWindowTitle( (SDL_Window*)(windowHandle_), Text::WideToNarrow( name_ ).c_str() );
    }

    void Window::SetWindowMode( const EWindowMode& mode )
    {
        if ( mode == WindowMode_Fullscreen )
            SDL_SetWindowFullscreen( (SDL_Window*)(windowHandle_), true );
        else if ( mode == WindowMode_Windowed )
        {
            SDL_SetWindowFullscreen( (SDL_Window*)(windowHandle_), false );
            if ( this->IsResizable() )
                SDL_RestoreWindow( (SDL_Window*)(windowHandle_) );
        }
        else if ( mode == WindowMode_Maximized )
        {
            SDL_SetWindowFullscreen( (SDL_Window*)(windowHandle_), false );
            if ( this->IsResizable() )
                SDL_MaximizeWindow( (SDL_Window*)(windowHandle_) );
        }
    }

    const EWindowMode& Window::GetWindowMode() const
    {
        return mode_;
    }

    const float& Window::GetWhiteLevel() const
    {
        return whiteLevel_;
    }

    const bool& Window::GetAllowHDR() const
    {
        return allowHDR_;
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
        SDL_SetWindowIcon( static_cast<SDL_Window*>(windowHandle_), Surface );
        SDL_DestroySurface( Surface );
    }

    void Window::Terminate()
    {
#ifdef EE_DEBUG
        EE_LOG_DEBUG( L"Window: \"{}\" closed!", GetName() );
#endif // EE_DEBUG

        SDL_RemoveEventWatch( WindowEventsHandler, (void*)this );
        GDynamicRHI->FreeRHIPresentContextOfWindow( this );
        SDL_DestroyWindow( (SDL_Window*)(windowHandle_) );
        windowHandle_ = NULL;
    }

    void Window::OnResize( const uint32& width, const uint32& height )
    {
        width_ = width; height_ = height;
    }

    void Window::OnPositionChange( const int32& x, const int32& y )
    {
        positionX_ = x; positionY_ = y;
    }

    void Window::OnWindowModeChanged( const EWindowMode& mode )
    {
        mode_ = mode;
    }

    const WString& Window::GetName() const
    {
        return name_;
    }

    float Window::GetAspectRatio() const
    {
        return (float)width_ / (float)height_;
    }

    const uint32& Window::GetWidth() const
    {
        return width_;
    }

    const uint32& Window::GetHeight() const
    {
        return height_;
    }

    const int32& Window::GetPositionX() const
    {
        return positionX_;
    }

    const int32& Window::GetPositionY() const
    {
        return positionY_;
    }

    const EPresentMode& Window::GetPresentMode() const
    {
        return presentMode_;
    }

    void Window::GetSize( uint32& w, uint32& h ) const
    {
        w = width_; h = height_;
    }

    void Window::GetPosition( int32& x, int32& y ) const
    {
        x = width_; y = height_;
    }

    void Window::GetViewport( int& x, int& y, int& w, int& h ) const
    {
        x = 0; y = 0; w = width_; h = height_;
    }
}
