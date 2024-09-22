
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
        if ( width_ < 0 )
            width_ = GetSystemMetrics( SM_CXSCREEN );
        if ( height_ < 0 )
            height_ = GetSystemMetrics( SM_CYSCREEN );
    }

    WindowsWindow::~WindowsWindow()
    {
        Super::~Window();
    }

    bool WindowsWindow::MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a )
    {
        HWND hWnd = (HWND)SDL_GetPointerProperty( SDL_GetWindowProperties( (SDL_Window*)GetWindowHandle() ), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL );

        SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong( hWnd, GWL_EXSTYLE ) | WS_EX_LAYERED );

        COLORREF colorKey = RGB( r, g, b );
        return SetLayeredWindowAttributes( hWnd, colorKey, a, LWA_ALPHA | LWA_COLORKEY );
    }

    Window* PlatformCreateWindow( const WindowCreateDescription& description )
    {
        return new WindowsWindow( description );
    }
}
