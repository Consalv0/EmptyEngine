
#include "RHI/RHI.h"

namespace EE
{
    enum class EWindowsRHI
    {
        Vulkan,
    };

    EDynamicRHI GetDefaultRHI( EWindowsRHI inWindowsRHI );
};