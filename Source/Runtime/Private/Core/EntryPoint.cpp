#pragma once

#include "CoreMinimal.h"
#include "Core/Application.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>

extern EEngine::Application* EEngine::CreateApplication();

#include "Platform/Platform.h"

int main(int argc, char **argv) 
{
	// EEngine::Log::Initialize();
	EEngine::Application::GetInstance()->Run();

#ifdef EE_DEBUG
	_getch();
#endif // ES_DEBUG

	return 0;
}