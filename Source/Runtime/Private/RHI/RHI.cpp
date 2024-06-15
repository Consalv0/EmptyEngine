
#include "CoreMinimal.h"

#include "RHI/RHI.h"

namespace EE
{
    DynamicRHI* GDynamicRHI = NULL;

    TMap<Window*, RHIPresentContext*> GWindowPresentContexts;

    const RHIPresentContext* CreatePresentContextOfWindow( Window* window )
    {
        RHIPresentContext* presentContext = GDynamicRHI->CreateRHIPresentContext( window );
        GWindowPresentContexts.emplace( window, presentContext );
        return GWindowPresentContexts[ window ];
    };

    const RHIPresentContext* GetPresentContextOfWindow( Window* window )
    {
        return GWindowPresentContexts[ window ];
    };

    void FreePresentContextOfWindow( Window* window )
    {
        delete GWindowPresentContexts[ window ];
        GWindowPresentContexts.erase( window );
    };

    DynamicRHI::DynamicRHI() { }
}