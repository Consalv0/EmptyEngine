#pragma once

#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Platform/Platform.h"

static void mainloop( void )
{
    if ( EE::GEngine->WantToTerminate() )
    {
        EE::GEngine->Terminate();

        delete EE::GEngine;
        EE::GEngine = nullptr;

#ifdef EE_DEBUG
        // _getch();
#endif // EE_DEBUG
        return;
    }

    EE::GEngine->MainLoop();
}

int main( int argc, char** argv )
{
#ifdef EE_PLATFORM_WINDOWS
    SetConsoleOutputCP( CP_UTF8 );
#endif

    EE::Log::Initialize();
    EE::GEngine = new EE::GameEngine();
    if ( EE::GEngine->Initialize( argc, argv ) )
    {
        EE::GEngine->Start();
    }

#ifdef EE_PLATFORM_WEB
    emscripten_set_main_loop( mainloop, 0, 1 );
#else
    do { mainloop(); } while ( EE::GEngine != nullptr );
#endif

    return EXIT_SUCCESS;
}