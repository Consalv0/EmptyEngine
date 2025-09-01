
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Window.h"

#include "Utils/TextFormatting.h"

#include "Platform/PrePlatform.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Platform/Windows/WindowsInput.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>

#include <dwmapi.h>
#include "Platform/PostPlatform.h"

namespace EE
{
    WindowsWindow::WindowsWindow( const WindowCreateDescription& description ) : Super::Window( description )
    {
    }

    WindowsWindow::~WindowsWindow()
    {
        Super::~Window();
    }

    bool WindowsWindow::SetOpacity( const uint8& opacity )
    {
        HWND hWnd = (HWND)SDL_GetPointerProperty( SDL_GetWindowProperties( (SDL_Window*)GetWindowHandle() ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL );
        
        LONG exStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
        if ( exStyle & WS_EX_TRANSPARENT )
        {
            _opacity = opacity;
            exStyle |= WS_EX_LAYERED;
            SetWindowLong( hWnd, GWL_EXSTYLE, exStyle );
            return SetLayeredWindowAttributes( hWnd, 0, _opacity, LWA_ALPHA );
        }
        else
        {
            _opacity = 255;
            exStyle &= ~WS_EX_LAYERED;
            return SetWindowLong( hWnd, GWL_EXSTYLE, exStyle );
        }
    }

    bool WindowsWindow::SetPassthrough( bool enable )
    {
        HWND hWnd = (HWND)SDL_GetPointerProperty( SDL_GetWindowProperties( (SDL_Window*)GetWindowHandle() ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL );

        COLORREF key = 0;
        BYTE alpha = 0;
        DWORD flags = 0;
        DWORD exStyle = GetWindowLong( hWnd, GWL_EXSTYLE );

        if ( exStyle & WS_EX_LAYERED )
            GetLayeredWindowAttributes( hWnd, &key, &alpha, &flags );

        if ( enable )
            exStyle |= (WS_EX_TRANSPARENT | WS_EX_LAYERED);
        else
        {
            exStyle &= ~WS_EX_TRANSPARENT;
            // NOTE: Window opacity also needs the layered window style so do not
            //       remove it if the window is alpha blended
            if ( exStyle & WS_EX_LAYERED )
            {
                if ( !(flags & LWA_ALPHA) )
                    exStyle &= ~WS_EX_LAYERED;
            }
        }

        _passthrough = enable;
        return SetWindowLong( hWnd, GWL_EXSTYLE, exStyle );
    }

    Window* PlatformCreateWindow( const WindowCreateDescription& description )
    {
        return new WindowsWindow( description );
    }
}