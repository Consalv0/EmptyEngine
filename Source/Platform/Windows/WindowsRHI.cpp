
#include "CoreMinimal.h"

#include "WindowsMinimal.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "Platform/Windows/WindowsRHI.h"

namespace EE
{
    EDynamicRHI GetDefaultRHI( EWindowsRHI InWindowsRHI )
    {
        switch ( InWindowsRHI )
        {
        case EWindowsRHI::Vulkan: return EDynamicRHI::Vulkan;
        default: return EDynamicRHI::Vulkan;
        }
    }

    DynamicRHI* PlatformCreateDynamicRHI()
    {
        EDynamicRHI windowsRHI = GetDefaultRHI( EWindowsRHI::Vulkan );

        switch ( windowsRHI )
        {
        case EDynamicRHI::Vulkan: return new VulkanRHI();
        default: EE_CORE_ASSERT( false, L"Failed to create Dynamic RHI!" );
        }

        return NULL;
    };
};