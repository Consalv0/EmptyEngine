
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
        if ( width_ == 0 )
            width_ = GetSystemMetrics( SM_CXSCREEN );
        if ( height_ == 0 )
            height_ = GetSystemMetrics( SM_CYSCREEN );

        if ( mode_ == WindowMode_Windowed )
        {
            height_ -= 1;
        }
    }

    WindowsWindow::~WindowsWindow()
    {
        Super::~Window();
    }

    void SetWindowMousePassthroughWin32( HWND hWnd, bool enabled )
    {
        COLORREF key = 0;
        BYTE alpha = 0;
        DWORD flags = 0;
        DWORD exStyle = GetWindowLongW( hWnd, GWL_EXSTYLE );

        if ( exStyle & WS_EX_LAYERED )
            GetLayeredWindowAttributes( hWnd, &key, &alpha, &flags );

        if ( enabled )
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

        SetWindowLongW( hWnd, GWL_EXSTYLE, exStyle );

        if ( enabled )
            SetLayeredWindowAttributes( hWnd, key, alpha, flags );
    }
    
    void SetWindowOpacityWin32( HWND hWnd, uint8 opacity )
    {
        LONG exStyle = GetWindowLongW( hWnd, GWL_EXSTYLE );
        if ( opacity < 255 || (exStyle & WS_EX_TRANSPARENT) )
        {
            MARGINS margins = { -1 };
            DwmExtendFrameIntoClientArea( hWnd, &margins );

            const BYTE alpha = (BYTE)(opacity);
            exStyle |= WS_EX_LAYERED;
            SetWindowLongW( hWnd, GWL_EXSTYLE, exStyle );
            SetLayeredWindowAttributes( hWnd, 0, alpha, LWA_ALPHA );
        }
        else if ( exStyle & WS_EX_TRANSPARENT )
        {
            SetLayeredWindowAttributes( hWnd, 0, 0, 0 );
        }
        else
        {
            exStyle &= ~WS_EX_LAYERED;
            SetWindowLongW( hWnd, GWL_EXSTYLE, exStyle );
        }
    }

    bool WindowsWindow::MakeTransparent( bool enable, const uint8& a )
    {
        HWND hWnd = (HWND)SDL_GetPointerProperty( SDL_GetWindowProperties( (SDL_Window*)GetWindowHandle() ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL );

        SetWindowMousePassthroughWin32( hWnd, true );
        SetWindowOpacityWin32( hWnd, a );
        return true;
    }

    Window* PlatformCreateWindow( const WindowCreateDescription& description )
    {
        return new WindowsWindow( description );
    }
}
