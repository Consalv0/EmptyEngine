#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/Application.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>

extern EE::Application* EE::CreateApplication();

extern EE::Window* EE::CreateApplicationWindow();

#include "Platform/Platform.h"

#ifdef EE_PLATFORM_WINDOWS
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#endif

int main(int argc, char **argv) 
{
#ifdef EE_PLATFORM_WINDOWS
    SetConsoleOutputCP( CP_UTF8 );
#endif

	// EE::Log::Initialize();
    EE::Log::Initialize();
	EE::GEngine = new EE::GameEngine();
    if ( EE::GEngine->Initialize() )
    {
        EE::GEngine->Run();
    }
    EE::GEngine->Terminate();

#ifdef EE_DEBUG
	// _getch();
#endif // EE_DEBUG

	delete EE::GEngine;

	return 0;
}