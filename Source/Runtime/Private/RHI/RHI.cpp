
#include "CoreMinimal.h"

#include "RHI/RHI.h"

namespace EE
{
    DynamicRHI* GDynamicRHI = NULL;

    TMap<Window*, RHIPresentContext> GWindowPresentContexts;

    const RHIPresentContext& GetPresentContextOfWindow( Window* window )
    {
        return GWindowPresentContexts[ window ];
    };

    const RHIPresentContext& CreatePresentContextOfWindow( Window* window )
    {
        GWindowPresentContexts.insert( std::make_pair( window, RHIPresentContext() ) );
        GDynamicRHI->CreatePresentContext( window, GWindowPresentContexts[ window ] );
        return GWindowPresentContexts[ window ];
    };

    void FreePresentContextOfWindow( Window* window )
    {
        GWindowPresentContexts.erase( window );
    };
}