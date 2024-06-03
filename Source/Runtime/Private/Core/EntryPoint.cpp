#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Platform/Platform.h"

int main( int argc, char** argv )
{
#ifdef EE_PLATFORM_WINDOWS
    SetConsoleOutputCP( CP_UTF8 );
#endif

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