
#include "CoreMinimal.h"

#include <Core/Collections.h>
#include <Math/CoreMath.h>
#include "RHI/RHI.h"

namespace EE
{
    DynamicRHI* GDynamicRHI = NULL;

    TMap<Window*, RHIPresentContext*> GWindowRHIPresentContexts;

    const RHIPresentContext* DynamicRHI::CreateRHIPresentContextOfWindow( Window* window ) const
    {
        RHIPresentContext* presentContext = GDynamicRHI->CreateRHIPresentContext( window );
        GWindowRHIPresentContexts.emplace( window, presentContext );
        return GWindowRHIPresentContexts[ window ];
    };

    const RHIPresentContext* DynamicRHI::GetRHIPresentContextOfWindow( Window* window ) const
    {
        return GWindowRHIPresentContexts[ window ];
    };

    void DynamicRHI::FreeRHIPresentContextOfWindow( Window* window ) const
    {
        RHIPresentContext* context = GWindowRHIPresentContexts[ window ];
        delete context;
        GWindowRHIPresentContexts.erase( window );
    };

    void DynamicRHI::BeginWindowRender( Window* window ) const
    {
        RHIPresentContext* presentContext = GWindowRHIPresentContexts[ window ];
        presentContext->AquireBackbuffer( UINT64_MAX );
    };

    void DynamicRHI::EndWindowRender( Window* window ) const
    {
        RHIPresentContext* presentContext = GWindowRHIPresentContexts[ window ];
        presentContext->Present();
    };

    DynamicRHI::DynamicRHI() { }
}