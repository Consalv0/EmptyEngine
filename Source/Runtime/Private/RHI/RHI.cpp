
#include "CoreMinimal.h"

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

    DynamicRHI::DynamicRHI() { }
}