
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Window.h"

#include "Utils/TextFormatting.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Platform/Windows/WindowsInput.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL.h>

#include <dwmapi.h>

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
        
        LONG exStyle = GetWindowLongW( hWnd, GWL_EXSTYLE );
        if ( exStyle & WS_EX_TRANSPARENT )
        {
            opacity_ = opacity;
            exStyle |= WS_EX_LAYERED;
            SetWindowLongW( hWnd, GWL_EXSTYLE, exStyle );
            return SetLayeredWindowAttributes( hWnd, 0, opacity_, LWA_ALPHA );
        }
        else
        {
            opacity_ = 255;
            exStyle &= ~WS_EX_LAYERED;
            return SetWindowLongW( hWnd, GWL_EXSTYLE, exStyle );
        }
    }

    bool WindowsWindow::SetPassthrough( bool enable )
    {
        HWND hWnd = (HWND)SDL_GetPointerProperty( SDL_GetWindowProperties( (SDL_Window*)GetWindowHandle() ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL );

        COLORREF key = 0;
        BYTE alpha = 0;
        DWORD flags = 0;
        DWORD exStyle = GetWindowLongW( hWnd, GWL_EXSTYLE );

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

        passthrough_ = enable;
        return SetWindowLongW( hWnd, GWL_EXSTYLE, exStyle );
    }

    Window* PlatformCreateWindow( const WindowCreateDescription& description )
    {
        return new WindowsWindow( description );
    }
}
