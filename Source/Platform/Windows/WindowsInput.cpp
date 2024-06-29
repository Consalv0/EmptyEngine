
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"
#include "Platform/Windows/WindowsWindow.h"

#include <SDL3/SDL_events.h>
#include "Platform/Windows/WindowsInput.h"

#include <SDL3/SDL.h>

namespace EE
{
    Input* GInput = NULL;

    Input* PlatformCreateInput()
    {
        return new WindowsInput();
    }
}