
#include "CoreMinimal.h"

#include "RHI/Vulkan/VulkanRHI.h"
#include "Platform/Windows/WindowsRHI.h"

#include "WindowsMinimal.h"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

namespace EE
{
    EDynamicRHI GetDefaultRHI( EDynamicRHI inRHI )
    {
        switch ( inRHI )
        {
        case EDynamicRHI::Vulkan: return EDynamicRHI::Vulkan;
        default: return EDynamicRHI::Vulkan;
        }
    }

    DynamicRHI* PlatformCreateDynamicRHI( EDynamicRHI preferedRHI )
    {
        EDynamicRHI windowsRHI = GetDefaultRHI( preferedRHI );

        switch ( windowsRHI )
        {
        case EDynamicRHI::Vulkan: return new VulkanRHI();
        default: EE_ASSERT( false, L"Failed to create Dynamic RHI!" );
        }

        return NULL;
    };

    const NChar* const GVulkanExtensionsForWin32[] =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME
    };

    void GetVulkanInstanceExtensions( uint32* count, const NChar* const*& extensions )
    {
        if ( count ) { *count = EE_ARRAYSIZE( GVulkanExtensionsForWin32 ); }
        extensions = (const NChar* const*)&GVulkanExtensionsForWin32;
    }
};