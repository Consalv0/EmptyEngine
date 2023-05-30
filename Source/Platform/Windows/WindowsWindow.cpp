
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

namespace EE
{
    WindowsWindow::WindowsWindow( const WindowProperties& Parameters ) : Super::Window( Parameters )
    {
    }

    WindowsWindow::~WindowsWindow()
    {
        Super::~Window();
    }

    Window* Window::Create( const WindowProperties& parameters )
    {
        return new WindowsWindow( parameters );
    }
}
