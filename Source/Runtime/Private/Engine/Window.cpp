
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
    SDL_bool WindowEventsHandler( void* userData, SDL_Event* sdlEvent )
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
            case SDL_EVENT_WINDOW_MOVED:
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
            windowFlags |= SDL_WINDOW_UTILITY | SDL_WINDOW_TRANSPARENT;
        }
        if ( mode_ == WindowMode_Fullscreen )
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        }
        if ( mode_ == WindowMode_Maximized )
        {
            windowFlags |= SDL_WINDOW_MAXIMIZED;
        }

        if ( (windowHandle_ = SDL_CreateWindow(
            Text::WideToNarrow( name_ ).c_str(),
            width_, height_,
            windowFlags
        )) == NULL )
        {
            EE_LOG_CRITICAL( L"Window: \"{0}\" could not be initialized: {1}", name_, Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        GDynamicRHI->CreateRHIPresentContextOfWindow( this );

        SDL_SetWindowKeyboardGrab( (SDL_Window*)windowHandle_, SDL_bool( false ) );

        eventData_ = Window::EventData
        {
            .window = this,
            .resizeEvent = [this]( const uint32& width, const uint32& height ) constexpr { OnResize( width, height ); },
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
        name_ = parameters.name;
        mode_ = parameters.windowMode;
        allowHDR_ = parameters.allowHDR;
        whiteLevel_ = parameters.whiteLevel;
        options_ = parameters.options;
        presentMode_ = parameters.presentMode;
        windowedWidth_ = width_;
        windowedHeight_ = height_;
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

    void Window::OnWindowModeChanged( const EWindowMode& mode )
    {
        mode_ = mode;
        if ( mode != WindowMode_Windowed )
            windowedWidth_ = width_; windowedHeight_ = height_;
        if ( mode == WindowMode_Windowed )
        {
            // Resize( windowedWidth_, windowedHeight_ );
        }
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

    const EPresentMode& Window::GetPresentMode() const
    {
        return presentMode_;
    }

    void Window::GetSize( uint32& w, uint32& h ) const
    {
        w = width_; h = height_;
    }

    void Window::GetViewport( int& x, int& y, int& w, int& h ) const
    {
        x = 0; y = 0; w = width_; h = height_;
    }

    bool Window::MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a )
    {
        return false;
    }
}
