
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Window.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/CPU/CPUGraphicsDevice.h"
#include "Graphics/PixelMap.h"

#include "Utils/TextFormatting.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>


namespace EE
{
    int WindowEventsHandler( void* userData, SDL_Event* sdlEvent )
    {
        Window& window = *(Window*)userData;

        if ( sdlEvent->type >= SDL_EVENT_WINDOW_FIRST && sdlEvent->type <= SDL_EVENT_WINDOW_LAST )
        {
            if ( sdlEvent->window.windowID != SDL_GetWindowID( (SDL_Window*)window.GetHandle() ) )
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
            GEngine->ShouldTerminate();
            break;
        }
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_FOCUS_LOST:
        {
            break;
        }
        }

        return 0;
    }

    bool Window::Initialize()
    {
        if ( windowHandle_ != NULL ) 
            return false;

        uint32 windowFlags = SDL_WINDOW_KEYBOARD_GRABBED;
        EWindowGraphicsAPI graphicsAPI = EE::GEngine->graphicsDevice_->GetWindowGraphicsAPI();

        if ( graphicsAPI == WindowGraphicsAPI_Vulkan )
        {
            windowFlags |= SDL_WINDOW_VULKAN;
        }

        if ( options_ & WindowOption_Resizable )
        {
            windowFlags |= SDL_WINDOW_RESIZABLE;
        }
        if ( options_ & WindowOption_Borderless )
        {
            windowFlags |= SDL_WINDOW_BORDERLESS;
        }
        if ( options_ & WindowOption_AlwaysOnTop )
        {
            windowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
        }
        if ( options_ & WindowOption_SkipTaskbar )
        {
            windowFlags |= SDL_WINDOW_UTILITY | SDL_WINDOW_TRANSPARENT;
        }

        if ( (windowHandle_ = SDL_CreateWindow(
            Text::WideToNarrow( name_ ).c_str(),
            width_, height_,
            windowFlags | mode_
        )) == NULL )
        {
            EE_LOG_CORE_CRITICAL( L"Window: \"{0}\" could not be initialized: {1}", name_, Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        EE::GEngine->graphicsDevice_->CreateWindowContext( this, windowContext_ );

        SDL_SetWindowKeyboardGrab( (SDL_Window*)windowHandle_, SDL_bool( false ) );
        SDL_AddEventWatch( WindowEventsHandler, (void*)this );
        return true;
    }

    Window::Window( const WindowProperties& parameters )
    {
        windowHandle_ = NULL;
        width_ = parameters.width;
        height_ = parameters.height;
        name_ = parameters.name;
        mode_ = parameters.windowMode;
        options_ = parameters.options;
    }

    Window::~Window()
    {
        Terminate();
    }

    void Window::Resize( const uint32& width, const uint32& height )
    {
        if ( width_ != width || height_ != height )
        {
            width_ = width; height_ = height;
            SDL_SetWindowSize( (SDL_Window*)(windowHandle_), width, height );
        }
    }

    void Window::SetName( const WString& newName )
    {
        name_ = newName;
        SDL_SetWindowTitle( (SDL_Window*)(windowHandle_), Text::WideToNarrow( name_ ).c_str() );
    }

    void Window::SetWindowMode( EWindowMode mode )
    {
        this->mode_ = mode;
        SDL_SetWindowFullscreen( (SDL_Window*)(windowHandle_), (SDL_bool)mode );
    }

    EWindowMode Window::GetWindowMode() const
    {
        return mode_;
    }

    void Window::SetIcon( PixelMap* Icon )
    {
        if ( Icon->GetColorFormat() != PixelFormat_R8G8B8A8 ) return;
        SDL_Surface* Surface = SDL_CreateSurfaceFrom(
            (void*)Icon->PointerToValue(),
            Icon->GetWidth(), Icon->GetHeight(),
            NULL, SDL_PIXELFORMAT_RGBA32
        );
        SDL_SetWindowIcon( static_cast<SDL_Window*>(windowHandle_), Surface );
        SDL_DestroySurface( Surface );
    }

    void Window::Terminate()
    {
#ifdef EE_DEBUG
        EE_LOG_CORE_DEBUG( L"Window: \"{}\" closed!", GetName() );
#endif // EE_DEBUG
        SDL_DestroyWindow( (SDL_Window*)(windowHandle_) );
        windowHandle_ = NULL;
        windowContext_ = WindowContext();
        SDL_DelEventWatch( WindowEventsHandler, (void*)this );
    }

    WString Window::GetName() const
    {
        return name_;
    }

    float Window::GetAspectRatio() const
    {
        return (float)width_ / (float)height_;
    }

    int32 Window::GetWidth() const
    {
        return width_;
    }

    int32 Window::GetHeight() const
    {
        return height_;
    }

    bool Window::GetVSync() const
    {
        return vsync_;
    }

    IntVector2 Window::GetSize() const
    {
        return IntVector2( width_, height_ );
    }

    IntBox2 Window::GetViewport() const
    {
        return IntBox2( 0, 0, width_, height_ );
    }

    void* Window::GetHandle() const
    {
        return windowHandle_;
    }

    bool Window::MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a )
    {
        return false;
    }
}
