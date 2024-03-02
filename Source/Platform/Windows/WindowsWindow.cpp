
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Window.h"

#include "Graphics/GraphicsDevice.h"
#include "Graphics/CPU/CPUGraphicsDevice.h"
#include "Graphics/PixelMap.h"

#include "Utils/TextFormatting.h"

#include "Platform/Windows/WindowsWindow.h"
#include "Platform/Windows/WindowsInput.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_syswm.h>
#include <dwmapi.h>

namespace EE
{
    WindowsWindow::WindowsWindow( const WindowProperties& properties ) : Super::Window( properties )
    {
        if ( width_ < 0 )
            width_ = GetSystemMetrics( SM_CXSCREEN );
        if ( height_ < 0 )
            height_ = GetSystemMetrics( SM_CYSCREEN );
        Initialize();
    }

    WindowsWindow::~WindowsWindow()
    {
        Super::~Window();
    }

    bool WindowsWindow::MakeTransparent( const uint8& r, const uint8& g, const uint8& b, const uint8& a )
    {
        SDL_SysWMinfo info;
        SDL_version version;
        SDL_VERSION( &version ); /* initialize info structure with SDL version info */

        SDL_GetWindowWMInfo( (SDL_Window*)windowHandle_, &info, *(Uint32*)(&version) );
        HWND hWnd = info.info.win.window;

        SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong( hWnd, GWL_EXSTYLE ) | WS_EX_LAYERED );

        COLORREF colorKey = RGB( r, g, b );
        return SetLayeredWindowAttributes( hWnd, colorKey, a, LWA_ALPHA | LWA_COLORKEY );
        return true;
    }

    Window* Window::Create( const WindowProperties& parameters )
    {
        return new WindowsWindow( parameters );
    }
}
