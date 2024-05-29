
#include "CoreMinimal.h"

#include "Engine/Engine.h"
#include "Engine/Video.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <dwmapi.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

namespace EE
{
    char const* const* Vulkan_GetInstanceExtensions( uint32* count )
    {
        static const char* const extensionsForWin32[] = {
            VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME
        };
        if ( count ) { *count = SDL_arraysize( extensionsForWin32 ); }
        return extensionsForWin32;
    }
}