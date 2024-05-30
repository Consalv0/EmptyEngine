
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Window.h"

#include "Rendering/RenderInterface.h"
#include "Rendering/PixelMap.h"

#include "Utils/TextFormatting.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>


namespace EE
{
    int WindowEventsHandler( void* userData, SDL_Event* sdlEvent )
    {
        Window* window = static_cast<Window*>( userData );

        if ( sdlEvent->type >= SDL_EVENT_WINDOW_FIRST && sdlEvent->type <= SDL_EVENT_WINDOW_LAST )
        {
            if ( sdlEvent->window.windowID != SDL_GetWindowID( (SDL_Window*)window->GetHandle() ) )
                return 0;
        }

        switch ( sdlEvent->type )
        {
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_RESIZED:
        {
            break;
        }

        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        {
            if ( sdlEvent->window.windowID == SDL_GetWindowID( (SDL_Window*)window->GetHandle() ) )
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
            if ( sdlEvent->window.windowID == SDL_GetWindowID( (SDL_Window*)window->GetHandle() ) )
                window->closeRequested = true;
            break;
        }
        }

        return 0;
    }

    bool Window::Initialize()
    {
        if ( windowHandle != NULL ) 
            return false;

        uint32 windowFlags = SDL_WINDOW_KEYBOARD_GRABBED;
        ERenderInterface renderingInterface = EE::GEngine->renderingInterface->GetType();

        if ( renderingInterface == RenderInterface_Vulkan )
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
            EE_LOG_CORE_CRITICAL( L"Window: \"{0}\" could not be initialized: {1}", name, Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        EE::GEngine->renderingInterface->CreatePresentContext( this, presentContext );

        // SDL_SetWindowKeyboardGrab( (SDL_Window*)windowHandle, SDL_bool( false ) );
        SDL_AddEventWatch( WindowEventsHandler, (void*)this );
        return true;
    }

    Window::Window( const WindowProperties& parameters )
    {
        closeRequested = false;
        windowHandle = NULL;
        width = parameters.width;
        height = parameters.height;
        name = parameters.name;
        mode = parameters.windowMode;
        allowHDR = parameters.allowHDR;
        options = parameters.options;
    }

    Window::~Window()
    {
        Terminate();
    }

    void Window::Resize( const uint32& width, const uint32& height )
    {
        if ( width != width || height != height )
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

    void Window::SetWindowMode( EWindowMode mode )
    {
        this->mode = mode;
        SDL_SetWindowFullscreen( (SDL_Window*)(windowHandle), (SDL_bool)mode );
    }

    EWindowMode Window::GetWindowMode() const
    {
        return mode;
    }

    bool Window::GetAllowHDR() const
    {
        return allowHDR;
    }

    void Window::SetIcon( PixelMap* Icon )
    {
        if ( Icon->GetColorFormat() != PixelFormat_R8G8B8A8_UINT ) return;
        SDL_Surface* Surface = SDL_CreateSurfaceFrom(
            (void*)Icon->PointerToValue(),
            Icon->GetWidth(), Icon->GetHeight(),
            NULL, SDL_PIXELFORMAT_RGBA32
        );
        SDL_SetWindowIcon( static_cast<SDL_Window*>(windowHandle), Surface );
        SDL_DestroySurface( Surface );
    }

    void Window::Terminate()
    {
#ifdef EE_DEBUG
        EE_LOG_CORE_DEBUG( L"Window: \"{}\" closed!", GetName() );
#endif // EE_DEBUG

        SDL_DelEventWatch( WindowEventsHandler, (void*)this );
        presentContext = PresentContext();
        SDL_DestroyWindow( (SDL_Window*)(windowHandle) );
        windowHandle = NULL;
    }

    WString Window::GetName() const
    {
        return name;
    }

    float Window::GetAspectRatio() const
    {
        return (float)width / (float)height;
    }

    int32 Window::GetWidth() const
    {
        return width;
    }

    int32 Window::GetHeight() const
    {
        return height;
    }

    bool Window::GetVSync() const
    {
        return vsync;
    }

    IntVector2 Window::GetSize() const
    {
        return IntVector2( width, height );
    }

    IntBox2 Window::GetViewport() const
    {
        return IntBox2( 0, 0, width, height );
    }

    void* Window::GetHandle() const
    {
        return windowHandle;
    }

    bool Window::MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a )
    {
        return false;
    }
}
