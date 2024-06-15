#pragma once

#include "RHI/RHI.h"

namespace EE
{
    class RHICommandContext
    {
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
    };
}