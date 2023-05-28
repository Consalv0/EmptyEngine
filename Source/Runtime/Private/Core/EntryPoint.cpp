#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/Application.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>

extern EE::Application* EE::CreateApplication();

#include "Platform/Platform.h"

int main(int argc, char **argv) 
{
	// EE::Log::Initialize();
	EE::GEngine = new EE::GameEngine();
    EE::GEngine->Initialize();
    EE::GEngine->Run();

#ifdef EE_DEBUG
	_getch();
#endif // EE_DEBUG

	delete EE::GEngine;

	return 0;
}