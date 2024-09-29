
#include "CoreMinimal.h"

#include <Engine/Engine.h>
#include <Platform/Platform.h>

#include <Core/Log.h>

namespace EE
{
    void GameEngine::ShowConsole( bool show )
    {
        HWND windowHandle = GetConsoleWindow();

        if ( bool(windowHandle == nullptr) == bool(show == false) )
            return;

        if ( show == false )
        {
            FreeConsole();
            Log::Shotdown();
        }
        else
        {
            AllocConsole();
            Log::Initialize();
        }
    }
}