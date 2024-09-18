
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Ticker.h"
#include "Engine/Window.h"

#include "RHI/RHI.h"
#include "Rendering/PixelMap.h"

#include "Utils/TextFormatting.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>


namespace EE
{
    SDL_bool WindowEventsHandler( void* userData, SDL_Event* sdlEvent )
    {
        Window* window = static_cast<Window*>( userData );

        if ( sdlEvent->type >= SDL_EVENT_WINDOW_FIRST && sdlEvent->type <= SDL_EVENT_WINDOW_LAST )
        {
            SDL_WindowID windowID = SDL_GetWindowID( (SDL_Window*)window->GetWindowHandle() );

            if ( sdlEvent->window.windowID != windowID )
                return true;

            // EE_LOG_INFO( L"SDL: Window Event: WindowID({}), Event({})", windowID, sdlEvent->type );

            switch ( sdlEvent->type )
            {
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                int w, h;
                SDL_GetWindowSizeInPixels( (SDL_Window*)window->GetWindowHandle(), &w, &h );
                window->Resize( w, h );
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED:
            {
                break;
            }

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            {
                window->closeRequested = true;
                break;
            }
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            case SDL_EVENT_WINDOW_FOCUS_LOST:
            {
                break;
            }
            case SDL_EVENT_WINDOW_DESTROYED:
            {
                window->closeRequested = true;
                break;
            }
            }
        }

        return true;
    }

    bool Window::Initialize()
    {
        if ( windowHandle != NULL ) 
            return false;

        uint32 windowFlags = SDL_WINDOW_KEYBOARD_GRABBED;
        EDynamicRHI renderingInterface = EE::GDynamicRHI->GetType();

        if ( renderingInterface == EDynamicRHI::Vulkan )
        {
            windowFlags |= SDL_WINDOW_VULKAN;
        }

        if ( options & WindowOption_Resizable )
        {
            windowFlags |= SDL_WINDOW_RESIZABLE;
        }
        if ( options & WindowOption_Borderless )
        {
            windowFlags |= SDL_WINDOW_BORDERLESS;
        }
        if ( options & WindowOption_AlwaysOnTop )
        {
            windowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
        }
        if ( options & WindowOption_SkipTaskbar )
        {
            windowFlags |= SDL_WINDOW_UTILITY | SDL_WINDOW_TRANSPARENT;
        }

        if ( (windowHandle = SDL_CreateWindow(
            Text::WideToNarrow( name ).c_str(),
            width, height,
            windowFlags | mode
        )) == NULL )
        {
            EE_LOG_CRITICAL( L"Window: \"{0}\" could not be initialized: {1}", name, Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        GDynamicRHI->CreateRHIPresentContextOfWindow( this );

        SDL_SetWindowKeyboardGrab( (SDL_Window*)windowHandle, SDL_bool( false ) );
        SDL_AddEventWatch( WindowEventsHandler, (void*)this );
        return true;
    }

    Window::Window( const WindowCreateDescription& parameters )
    {
        closeRequested = false;
        windowHandle = NULL;
        width = parameters.width;
        height = parameters.height;
        name = parameters.name;
        mode = parameters.windowMode;
        allowHDR = parameters.allowHDR;
        whiteLevel = parameters.whiteLevel;
        options = parameters.options;
        presentMode = parameters.presentMode;
    }

    Window::~Window()
    {
        Terminate();
    }

    void Window::Resize( const uint32& width, const uint32& height )
    {
        if ( this->width != width || this->height != height )
        {
            this->width = width; this->height = height;
            SDL_SetWindowSize( (SDL_Window*)(windowHandle), width, height );
        }
    }

    void Window::SetName( const WString& newName )
    {
        name = newName;
        SDL_SetWindowTitle( (SDL_Window*)(windowHandle), Text::WideToNarrow( name ).c_str() );
    }

    void Window::SetWindowMode( const EWindowMode& mode )
    {
        this->mode = mode;
        SDL_SetWindowFullscreen( (SDL_Window*)(windowHandle), (SDL_bool)mode );
    }

    const EWindowMode& Window::GetWindowMode() const
    {
        return mode;
    }

    const float& Window::GetWhiteLevel() const
    {
        return whiteLevel;
    }

    const bool& Window::GetAllowHDR() const
    {
        return allowHDR;
    }

    void Window::SetIcon( PixelMap* Icon )
    {
        if ( Icon->GetColorFormat() != PixelFormat_R8G8B8A8_UINT ) return;
        SDL_Surface* Surface = SDL_CreateSurfaceFrom(
            Icon->GetWidth(), Icon->GetHeight(),
            SDL_PIXELFORMAT_RGBA32,
            (void*)Icon->PointerToValue(),
            NULL
        );
        SDL_SetWindowIcon( static_cast<SDL_Window*>(windowHandle), Surface );
        SDL_DestroySurface( Surface );
    }

    void Window::Terminate()
    {
#ifdef EE_DEBUG
        EE_LOG_DEBUG( L"Window: \"{}\" closed!", GetName() );
#endif // EE_DEBUG

        SDL_RemoveEventWatch( WindowEventsHandler, (void*)this );
        GDynamicRHI->FreeRHIPresentContextOfWindow( this );
        SDL_DestroyWindow( (SDL_Window*)(windowHandle) );
        windowHandle = NULL;
    }

    const WString& Window::GetName() const
    {
        return name;
    }

    float Window::GetAspectRatio() const
    {
        return (float)width / (float)height;
    }

    const int32& Window::GetWidth() const
    {
        return width;
    }

    const int32& Window::GetHeight() const
    {
        return height;
    }

    const EPresentMode& Window::GetPresentMode() const
    {
        return presentMode;
    }

    void Window::GetSize( int& w, int& h ) const
    {
        w = width; h = height;
    }

    void Window::GetViewport( int& x, int& y, int& w, int& h ) const
    {
        x = 0; y = 0; w = width; h = height;
    }

    bool Window::MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a )
    {
        return false;
    }
}
