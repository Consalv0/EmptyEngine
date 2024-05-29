
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"
#include "Engine/Video.h"
#include "Rendering/Vulkan/VulkanRenderInterface.h"

#include <vk_mem_alloc.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <optional>
#include <set>
#include <map>

// --- Make discrete GPU by default.
extern "C" {
	// --- developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	// --- developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}

#define ToInternal( x, type ) static_cast<Vulkan##type*>( x.internalState.get() )

namespace EE
{
    static VkShaderStageFlagBits Vulkan_ConvertShaderStage( EShaderStage shaderStage )
    {
        switch ( shaderStage )
        {
        case ShaderStage_Tesselation:       return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage_TesselationEval:   return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case ShaderStage_Vertex:            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage_Geometry:          return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage_Pixel:             return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage_Compute:           return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_ALL;
        }
    }

    static VkFormat Vulkan_ConvertPixelFormat( EPixelFormat format, EColorSpace colorspace )
    {
        switch ( format )
        {
        case PixelFormat_R8G8_UNORM:
            if ( colorspace == ColorSpace_Linear ) {       return VK_FORMAT_R8G8_UNORM; }
                                                           return VK_FORMAT_R8G8_SRGB;
        case PixelFormat_R16G16B16A16_SNORM:               return VK_FORMAT_R16G16B16A16_SFLOAT;
        case PixelFormat_A2R10G10B10_UNORM:                return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case PixelFormat_R8G8B8A8_UNORM:
            if ( colorspace == ColorSpace_Linear ) {       return VK_FORMAT_B8G8R8A8_UNORM; }
                                                           return VK_FORMAT_B8G8R8A8_SRGB;
        case PixelFormat_DepthComponent24:                 return VK_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat_DepthStencil:                     return VK_FORMAT_D16_UNORM_S8_UINT;
        case PixelFormat_ShadowDepth:                      return VK_FORMAT_D16_UNORM;
        default:
            return VK_FORMAT_UNDEFINED;
        }
    }

    static EPixelFormat Vulkan_ToPixelFormat( VkFormat vkFormat )
    {
        switch ( vkFormat )
        {
        case VK_FORMAT_B8G8R8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_B8G8R8A8_SRGB:               return PixelFormat_R8G8B8A8_UNORM;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:    return PixelFormat_A2R10G10B10_UNORM;
        case VK_FORMAT_R16G16B16A16_SFLOAT:         return PixelFormat_R16G16B16A16_SNORM;
        default:
            return PixelFormat_Unknown;
        }
    }

    static EColorSpace Vulkan_ToColorSpace( VkColorSpaceKHR colorspace )
    {
        switch ( colorspace )
        {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:             return ColorSpace_sRGB;
        case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:       return ColorSpace_sRGB;
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:       return ColorSpace_sRGB;
        case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:          return ColorSpace_Linear;
        case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:           return ColorSpace_sRGB;
        case VK_COLOR_SPACE_BT709_LINEAR_EXT:               return ColorSpace_Linear;
        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:            return ColorSpace_sRGB;
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:              return ColorSpace_Linear;
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:               return ColorSpace_sRGB;
        case VK_COLOR_SPACE_DOLBYVISION_EXT:                return ColorSpace_sRGB;
        case VK_COLOR_SPACE_HDR10_HLG_EXT:                  return ColorSpace_sRGB;
        case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:            return ColorSpace_Linear;
        case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:         return ColorSpace_sRGB;
        case VK_COLOR_SPACE_PASS_THROUGH_EXT:               return ColorSpace_sRGB;
        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:    return ColorSpace_sRGB;
        case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD:             return ColorSpace_sRGB;
        default:
            return ColorSpace_sRGB;
        }
    }

    static bool Vulkan_FrameColorSpaceIsHDR( VkColorSpaceKHR colorspace )
    {
        switch ( colorspace )
        {
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:
        case VK_COLOR_SPACE_DOLBYVISION_EXT:
        case VK_COLOR_SPACE_HDR10_HLG_EXT:
            return true;
        default:
            return false;
        }
    }

    VkImageTiling Vulkan_ConvertTextureTiling( ETilingMode tiling )
    {
        switch ( tiling )
        {
        case TilingMode_Default:    return VK_IMAGE_TILING_OPTIMAL;
        case TilingMode_Optimal:    return VK_IMAGE_TILING_OPTIMAL;
        case TilingMode_Linear:     return VK_IMAGE_TILING_LINEAR;
        default:
            break;
        }

        return VK_IMAGE_TILING_OPTIMAL;
    }

    VkImageUsageFlags Vulkan_ConvertTextureUsage( EUsageMode usage )
    {
        switch ( usage )
        {
        case UsageMode_Sampled:         return VK_IMAGE_USAGE_SAMPLED_BIT;
        case UsageMode_Storage:         return VK_IMAGE_USAGE_STORAGE_BIT;
        case UsageMode_Color:           return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        case UsageMode_DepthStencil:    return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        default:
            return VK_IMAGE_USAGE_SAMPLED_BIT;
        }
    }

    VkSharingMode Vulkan_ConvertSharingMode( ESharingMode sharing )
    {
        switch ( sharing )
        {
        case EE::SharingMode_Dynamic:       return VK_SHARING_MODE_CONCURRENT;
        case EE::SharingMode_Static:        return VK_SHARING_MODE_EXCLUSIVE;
        case EE::SharingMode_Default:       return VK_SHARING_MODE_EXCLUSIVE;
        default:
            return VK_SHARING_MODE_EXCLUSIVE;
        }
    }

    VkBufferUsageFlags Vulkan_ConvertBufferUsageFlags( EBufferUsageFlags usages )
    {
        switch ( usages )
        {
        case BufferUsage_SourceCopy:        return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        case BufferUsage_StreamOutput:      return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        case BufferUsage_Index:             return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case BufferUsage_Vertex:            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case BufferUsage_Uniform:           return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case BufferUsage_Storage:           return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        case BufferUsage_Indirect:          return VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        default:
            return 0;
        }
    }

    struct VulkanRenderInstance
    {
        VkInstance instance;

        std::vector<VkPhysicalDevice> physicalDevices;
        VkPhysicalDevice selectedDevice;

        ~VulkanRenderInstance()
        {
            vkDestroyInstance( instance, nullptr );
        }
    };

    struct VulkanRenderDevice
    {
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VmaAllocator allocator;

        uint32 queueFamilyCount;
        std::vector<VkQueueFamilyProperties> queueFamilies;

        union
        {
            uint32 graphicsQueueIndex;
            uint32 presentQueueIndex;
            uint32 queueFamilyIndices[ 2 ];
        };

        VkQueue graphicsQueue;
        VkQueue presentQueue;

        ~VulkanRenderDevice()
        {
            vmaDestroyAllocator( allocator );
            vkDestroyDevice( device, nullptr );
        }
    };

    struct VulkanPresentContext
    {
    };

    struct VulkanBuffer : public Buffer
    {
        VkDevice device;
        VkBuffer buffer;
        VmaAllocation nativeAllocation;
        EBufferUsageFlags usages;

        ~VulkanBuffer()
        {
            if ( buffer != NULL )
            {
                vkDestroyBuffer( device, buffer, NULL );
            }
        }
    };

    struct VulkanTexture
    {
        VkDevice device = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImage resource = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VmaAllocation memory = VK_NULL_HANDLE;

        ~VulkanTexture()
        {
            if ( resource != NULL )
            {
                vkDestroyImage( device, resource, NULL );
            }

            vkDestroyImageView( device, imageView, NULL );
        }
    };

    struct VulkanSwapChain
    {
        VkDevice device = VK_NULL_HANDLE;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VkExtent2D size = {};
        uint32 imageCount = 0;
        std::vector<VkImage> images;
        std::vector<Texture> imageTextures;

        ~VulkanSwapChain()
        {
            for ( uint32 i = 0; i < imageCount; i++ )
            {
                images[ i ] = NULL;
                ToInternal( imageTextures[ i ], Texture )->resource = NULL;
            }

            vkDestroySwapchainKHR( device, swapChain, NULL );
        }
    };

    struct VulkanSurface
    {
        VkInstance instance;
        uint32 formatCount = 0;
        std::vector<VkSurfaceFormatKHR> formats;
        VkSurfaceCapabilitiesKHR capabilities = {};
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        ~VulkanSurface()
        {
            vkDestroySurfaceKHR( instance, surface, nullptr);
        }
    };

    struct VulkanShaderStage
    {
        VkDevice device = VK_NULL_HANDLE;
        VkShaderModule shaderModule;
        
        ~VulkanShaderStage()
        {
            vkDestroyShaderModule( device, shaderModule, nullptr );
        }
    };
    
    struct QueueFamilyIndices
    {
        std::optional<uint32> graphicsFamily;
        std::optional<uint32> presentFamily;
        
        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    }; 
    
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities = {};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    VulkanRHI::~VulkanRHI()
    {
        if ( initialized )
        {
            device.internalState.reset();
            instance.internalState.reset();

            SDL_Vulkan_UnloadLibrary();
        }
    }

    ERenderInterface VulkanRHI::GetType() const
    {
        return RenderInterface_Vulkan;
    }

    bool IsDeviceSuitable( VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface );

    int RateDeviceSuitability( VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface )
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties( physicalDevice, &deviceProperties );
        vkGetPhysicalDeviceFeatures( physicalDevice, &deviceFeatures );

        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if ( deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
        {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if ( deviceFeatures.geometryShader == VK_FALSE )
        {
            return 0;
        }

        if ( IsDeviceSuitable( physicalDevice, windowSurface ) == false )
        {
            return 0;
        }

        EE_LOG_CORE_INFO(
            L"\u2570\u2500> {0} : {1}",
            Text::NarrowToWide( deviceProperties.deviceName ),
            score
        );

        return score;
    }

    VkPhysicalDevice PickPhysicalDevice( const std::vector<VkPhysicalDevice>& physicalDevices, VkSurfaceKHR surface )
    {
        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, const VkPhysicalDevice> candidates;

        for ( const auto& device : physicalDevices )
        {
            int score = RateDeviceSuitability( device, surface );
            candidates.insert( std::make_pair( score, device ) );
        }

        // Check if the best candidate is suitable at all
        if ( candidates.rbegin()->first > 0 )
        {
            return candidates.rbegin()->second;
        }
        else
        {
            return NULL;
        }
    }
    
    QueueFamilyIndices FindQueueFamilies( VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface )
    {
        QueueFamilyIndices indices;

        // Logic to find queue family indices to populate struct with
        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, nullptr );
        std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, queueFamilies.data() );
        
        int i = 0;
        for ( const auto& queueFamily : queueFamilies )
        {
            if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
            {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, i, windowSurface, &presentSupport );
            if ( presentSupport )
            {
                indices.presentFamily = i;
            }

            if ( indices.isComplete() )
            {
                break;
            }

            i++;
        }

        return indices;
    }


    const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const float QueuePriorities[] = { 1.0F };
    
    bool CheckDeviceExtensionSupport( VkPhysicalDevice physicalDevice )
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, nullptr );

        std::vector<VkExtensionProperties> availableExtensions( extensionCount );
        vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, &extensionCount, availableExtensions.data() );

        std::set<std::string> requiredExtensions( DeviceExtensions.begin(), DeviceExtensions.end() );

        for ( const auto& extension : availableExtensions )
        {
            requiredExtensions.erase( extension.extensionName );
        }

        return requiredExtensions.empty();
    }

    SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface )
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, windowSurface, &details.capabilities );

        uint32 formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, windowSurface, &formatCount, nullptr );
        if ( formatCount != 0 )
        {
            details.formats.resize( formatCount );
            vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, windowSurface, &formatCount, details.formats.data() );
        }

        uint32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, windowSurface, &presentModeCount, nullptr );

        if ( presentModeCount != 0 )
        {
            details.presentModes.resize( presentModeCount );
            vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, windowSurface, &presentModeCount, details.presentModes.data() );
        }

        return details;
    }
    
    bool IsDeviceSuitable( VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface )
    {
        QueueFamilyIndices indices = FindQueueFamilies( physicalDevice, windowSurface );

        bool extensionsSupported = CheckDeviceExtensionSupport( physicalDevice );

        bool swapChainAdequate = false;
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( physicalDevice, windowSurface );
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }
    
    uint32 FindMemoryType( VkPhysicalDevice physicalDevice, uint32 typeFilter, VkMemoryPropertyFlags properties )
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties( physicalDevice, &memProperties );

        for ( uint32 i = 0; i < memProperties.memoryTypeCount; i++ )
        {
            if ( (typeFilter & (1 << i)) && (memProperties.memoryTypes[ i ].propertyFlags & properties) == properties )
            {
                return i;
            }
        }

        EE_LOG_CORE_CRITICAL( L"Failed to find suitable memory type!" );
        return -1;
    }

    bool CreateNativeVmaAllocator( VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator* outAllocator )
    {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo info = {};
        info.vulkanApiVersion = VK_API_VERSION_1_3;
        info.instance = instance;
        info.physicalDevice = physicalDevice;
        info.device = device;
        info.pVulkanFunctions = &vulkanFunctions;

        return vmaCreateAllocator( &info, outAllocator );
    }

    bool SelectDevice( VulkanRenderInstance* instance, VkSurfaceKHR surface )
    {
        // Get physical device
        uint32 physicalDeviceCount;
        vkEnumeratePhysicalDevices( instance->instance, &physicalDeviceCount, nullptr);
        if ( physicalDeviceCount == 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find GPUs with Vulkan support!" );
            return false;
        }

        EE_LOG_CORE_INFO( L"\u250C> Available devices : {0}", physicalDeviceCount );

        // Find suitable device
        instance->physicalDevices.resize( physicalDeviceCount );
        vkEnumeratePhysicalDevices( instance->instance, &physicalDeviceCount, instance->physicalDevices.data() );
        VkPhysicalDevice selection = PickPhysicalDevice( instance->physicalDevices, surface );

        if ( selection == VK_NULL_HANDLE )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find a suitable device!" );
            return false;
        }

        instance->selectedDevice = selection;

        return true;
    }

    bool VulkanRHI::Initialize()
    {
        if ( initialized )
            return false;
        
        instance.internalState = std::make_shared<VulkanRenderInstance>();

        VkApplicationInfo appInfo = 
        {
            VK_STRUCTURE_TYPE_APPLICATION_INFO,
            NULL,
            "Vulkan",
            VK_MAKE_VERSION( 1, 0, 0 ),
            "Empty Engine",
            VK_MAKE_VERSION( 1, 0, 0 ),
            VK_API_VERSION_1_3
        };

        SDL_Vulkan_LoadLibrary( nullptr );

        uint32 extensionCount;
        const char* const* extensionNames = Vulkan_GetInstanceExtensions( &extensionCount );

        VkInstanceCreateInfo createInfo
        {
            VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,     // sType
            NULL,                                       // pNext
            0,                                          // flags
            &appInfo,                                   // pApplicationInfo
            0,                                          // enabledLayerCount
            NULL,                                       // ppEnabledLayerNames
            extensionCount,                             // enabledExtensionCount
            extensionNames                              // ppEnabledExtensionNames
        };

        VkResult createResult;
        if ( (createResult = vkCreateInstance( &createInfo, nullptr, &ToInternal( instance, RenderInstance )->instance ) ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"vkCreateInstance failed : {0}", (int32)createResult );
            return false;
        }

        return initialized = true;
    }

    bool VulkanRHI::CreateDevice( Surface& surface, RenderDevice& outDevice ) const
    {
        auto internalState = std::make_shared<VulkanRenderDevice>();
        internalState->physicalDevice = ToInternal( instance, RenderInstance )->selectedDevice;

        // Logic to find queue family indices to populate struct with
        vkGetPhysicalDeviceQueueFamilyProperties( internalState->physicalDevice, &internalState->queueFamilyCount, nullptr );
        internalState->queueFamilies.resize( internalState->queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties( internalState->physicalDevice, &internalState->queueFamilyCount, internalState->queueFamilies.data() );

        // Specifying the queues to be created
        QueueFamilyIndices indices = FindQueueFamilies( internalState->physicalDevice, ToInternal( surface, Surface )->surface );

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = QueuePriorities[ 0 ];
        for ( uint32 queueFamily : uniqueQueueFamilies )
        {
            VkDeviceQueueCreateInfo queueCreateInfo
            {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,     // sType
                NULL,                                           // pNext
                0,                                              // flags
                queueFamily,                                    // queueFamilyIndex
                1,                                              // queueCount
                &queuePriority                                  // pQueuePriorities
            };
            queueCreateInfos.push_back( queueCreateInfo );
        }

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = internalState->graphicsQueueIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{ };
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo deviceCreateInfo
        {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,           // sType
            NULL,                                           // pNext
            0,                                              // flags
            static_cast<uint32>(queueCreateInfos.size()),   // queueCreateInfoCount
            queueCreateInfos.data(),                        // pQueueCreateInfos
            0,                                              // enabledLayerCount
            NULL,                                           // ppEnabledLayerNames
            (uint32)DeviceExtensions.size(),                // enabledExtensionCount
            DeviceExtensions.data(),                        // ppEnabledExtensionNames
            &deviceFeatures                                 // pEnabledFeatures
        };

        EE_CORE_ASSERT
        (
            vkCreateDevice( internalState->physicalDevice, &deviceCreateInfo, nullptr, &internalState->device ) == VK_SUCCESS,
            L"Failed to create logical device!"
        );

        EE_CORE_ASSERT
        (
            CreateNativeVmaAllocator( ToInternal( instance, RenderInstance )->instance, internalState->physicalDevice, internalState->device, &internalState->allocator ) == VK_SUCCESS,
            L"Failed to create VMA Allocator!"
        );

        vkGetDeviceQueue( internalState->device, internalState->graphicsQueueIndex = indices.graphicsFamily.value(), 0, &internalState->graphicsQueue );
        vkGetDeviceQueue( internalState->device, internalState->presentQueueIndex  = indices.presentFamily.value(),  0, &internalState->presentQueue  );

        outDevice.internalState = internalState;
        return true;
    }

    bool VulkanRHI::CreatePresentContext( Window* window, PresentContext& outContext )
    {
        auto internalState = std::make_shared<VulkanPresentContext>();

        SurfaceDescription surfaceDescription;
        surfaceDescription.window = window;
        if ( CreateSurface( surfaceDescription, outContext.surface ) == false )
        {
            return false;
        }

        // Vulkan needs the surface information in order to determine the driver, so the driver creation is here, the device selection is handled inside CreateSurface
        if ( device.internalState == NULL )
        {
            if ( CreateDevice( outContext.surface, device ) == false )
            {
                return false;
            }
        }

        VulkanSurface* surface = ToInternal( outContext.surface, Surface );
        
        bool tryHDR = false;
        if ( window->GetAllowHDR() && SDL_GetBooleanProperty( SDL_GetDisplayProperties( SDL_GetPrimaryDisplay() ), SDL_PROP_DISPLAY_HDR_ENABLED_BOOLEAN, SDL_FALSE ) )
        {
            tryHDR = true;
        }

        EPixelFormat desiredFormat = PixelFormat_R8G8B8A8_UNORM;
        EColorSpace desiredColorSpace = ColorSpace_sRGB;
        // Find best format
        bool contains = false;
        for ( uint32 i = 0; i < surface->formatCount; i++ )
        {
            VkSurfaceFormatKHR& surfaceFormat = surface->formats[ i ];
            bool colorSpaceHDR = Vulkan_FrameColorSpaceIsHDR( surfaceFormat.colorSpace );
            EColorSpace colorSpace = Vulkan_ToColorSpace( surfaceFormat.colorSpace );
            EPixelFormat format = Vulkan_ToPixelFormat( surfaceFormat.format );

            if ( tryHDR )
            {
                if ( colorSpaceHDR )
                {
                    desiredFormat = format;
                    desiredColorSpace = colorSpace;
                    break;
                }

                if ( colorSpace == ColorSpace_sRGB )
                {
                    if ( format == PixelFormat_A2R10G10B10_UNORM || format == PixelFormat_R16G16B16A16_SNORM )
                    {
                        desiredFormat = format;
                        desiredColorSpace = colorSpace;
                        break;
                    }
                }

                if ( colorSpaceHDR == false )
                {
                    desiredFormat = format;
                    desiredColorSpace = colorSpace;
                }
            }
            else
            {
                if ( colorSpaceHDR )
                {
                    desiredFormat = format;
                    desiredColorSpace = colorSpace;
                    break;
                }
            }
        }

        SwapChainDescription swapChainDesc{};
        swapChainDesc.width = window->GetWidth();
        swapChainDesc.height = window->GetHeight();
        swapChainDesc.fullscreen = window->GetWindowMode();
        swapChainDesc.format = desiredFormat;
        swapChainDesc.colorSpace = desiredColorSpace;
        swapChainDesc.vsync = window->GetVSync();
        if ( CreateSwapChain( swapChainDesc, window, outContext.swapChain ) == false )
        {
            return false;
        }
        VulkanSwapChain* swapChain = ToInternal( outContext.swapChain, SwapChain );

        outContext.internalState = internalState;
        return true;
    }

    bool VulkanRHI::CreateSurface( const SurfaceDescription& description, Surface& outSurface ) const
    {
        auto internalState = std::make_shared<VulkanSurface>();
        internalState->surface = VkSurfaceKHR();
        internalState->instance = ToInternal( instance, RenderInstance )->instance;

        if ( SDL_Vulkan_CreateSurface( (SDL_Window*)description.window->GetWindowHandle(), internalState->instance, VK_NULL_HANDLE, &internalState->surface ) == SDL_FALSE )
        {
            EE_LOG_CORE_CRITICAL( L"SDL_Vulkan_CreateSurface failed! {0}", Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        if ( device.internalState == NULL )
        {
            if ( SelectDevice( ToInternal( instance, RenderInstance ), internalState->surface ) == false )
            {
                return false;
            }
        }

        VkPhysicalDevice physicalDevice = ToInternal( instance, RenderInstance )->selectedDevice;
        if ( IsDeviceSuitable( physicalDevice, internalState->surface) == false )
        {
            EE_LOG_CORE_CRITICAL( L"Current device is not suitable device for window!" );
            return false;
        }

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, internalState->surface, &internalState->capabilities );

        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, internalState->surface, &internalState->formatCount, NULL );
        internalState->formats.resize( internalState->formatCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, internalState->surface, &internalState->formatCount, internalState->formats.data() );

        description.window;
        outSurface.type = RenderingResourceType_Surface;
        outSurface.internalState = internalState;
        outSurface.description = description;

        return true;
    }

    bool CreateImageView( VkDevice device, const TextureDescription& description, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* outImageView )
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if ( vkCreateImageView( device, &viewInfo, nullptr, outImageView ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create texture image view!" );
            return false;
        }

        return true;
    }

    #define CLAMP(x, lo, hi)    ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
    bool VulkanRHI::CreateSwapChain( const SwapChainDescription& description, Window* window, SwapChain& outSwapChain ) const
    {
        VulkanRenderDevice* internalDevice = ToInternal( device, RenderDevice );
        auto internalState = std::make_shared<VulkanSwapChain>();
        internalState->device = internalDevice->device;

        auto presentContext = window->GetPresentContext();
        auto surface = ToInternal( presentContext.surface, Surface );

        VkSurfaceFormatKHR desiredFormat;

        bool contains = false;
        for ( uint32 i = 0; i < surface->formatCount; i++ )
        {
            VkSurfaceFormatKHR& surfaceFormat = surface->formats[ i ];
            if ( Vulkan_ToPixelFormat( surfaceFormat.format ) == description.format )
            {
                if ( Vulkan_ToColorSpace( surfaceFormat.colorSpace ) == description.colorSpace )
                {
                    contains = true;
                    desiredFormat = surfaceFormat;
                }
            }
        }

        if ( contains == false )
        {
            EE_LOG_CORE_CRITICAL( L"Surface Format {} is not supported!", (uint32)description.format, (uint32)description.colorSpace );
            return false;
        }

        int width, height = 0;
        SDL_GetWindowSizeInPixels( (SDL_Window*)window->GetWindowHandle(), &width, &height);
        width = CLAMP( (uint32)width, surface->capabilities.minImageExtent.width, surface->capabilities.maxImageExtent.width );
        height = CLAMP( (uint32)height, surface->capabilities.minImageExtent.height, surface->capabilities.maxImageExtent.height );
        internalState->size.width = width;
        internalState->size.height = height;

        uint32 imageCount = surface->capabilities.minImageCount + 1;
        if ( surface->capabilities.maxImageCount > 0 && imageCount > surface->capabilities.maxImageCount )
        {
            imageCount = surface->capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface->surface;
        createInfo.minImageCount = surface->capabilities.minImageCount;
        createInfo.imageFormat = desiredFormat.format;
        createInfo.imageColorSpace = desiredFormat.colorSpace;
        createInfo.imageExtent = internalState->size;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if ( internalDevice->graphicsQueueIndex != internalDevice->presentQueueIndex )
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = internalDevice->queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = surface->capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        vkCreateSwapchainKHR( internalState->device, &createInfo, NULL, &internalState->swapChain );

        vkGetSwapchainImagesKHR( internalState->device, internalState->swapChain, &internalState->imageCount, NULL );
        internalState->images.resize( internalState->imageCount );
        vkGetSwapchainImagesKHR( internalState->device, internalState->swapChain, &internalState->imageCount, internalState->images.data() );

        internalState->imageTextures.resize( internalState->imageCount );
        for ( uint32 i = 0; i < internalState->imageCount; i++ )
        {
            auto textureInternalState = std::make_shared<VulkanTexture>();
            textureInternalState->device = internalState->device;
            textureInternalState->resource = internalState->images[ i ];

            Texture& texture = internalState->imageTextures[ i ];
            texture.description.type = TextureType_Texture2D;
            texture.description.format = description.format;
            texture.description.colorSpace = description.colorSpace;
            texture.description.height = height;
            texture.description.width = width;
            texture.description.arraySize = 1;
            texture.description.mipLevels = 1;

            CreateImageView( internalState->device, texture.description, internalState->images[ i ], desiredFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &textureInternalState->imageView );

            texture.type = RenderingResourceType_Texture;
            texture.internalState = textureInternalState;
        }

        outSwapChain.type = RenderingResourceType_Swapchain;
        outSwapChain.internalState = internalState;

        return true;
	}

	bool VulkanRHI::CreateTexture( const TextureDescription& description, Texture& outTexture ) const
    {
        VulkanRenderDevice* internalDevice = ToInternal( device, RenderDevice );
        auto internalState = std::make_shared<VulkanTexture>();
        internalState->device = internalDevice->device;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = description.width;
        imageInfo.extent.height = description.height;
        imageInfo.extent.depth = description.depth;
        imageInfo.mipLevels = description.mipLevels;
        imageInfo.arrayLayers = description.arraySize;
        imageInfo.format = Vulkan_ConvertPixelFormat( description.format, description.colorSpace );
        imageInfo.tiling = Vulkan_ConvertTextureTiling( description.tiling );
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = Vulkan_ConvertTextureUsage( description.usage );
        imageInfo.sharingMode = Vulkan_ConvertSharingMode( description.sharing );
        imageInfo.samples = (VkSampleCountFlagBits)description.sampleCount;

        if ( vkCreateImage( internalState->device, &imageInfo, VK_NULL_HANDLE, &internalState->resource ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create image!" );
            return false;
        }

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        if ( vmaCreateImage( internalDevice->allocator, &imageInfo, &allocInfo, &internalState->resource, &internalState->memory, NULL ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to allocate image memory!" );
            return false;
        }

        // VkMemoryRequirements memRequirements;
        // vkGetImageMemoryRequirements( internalState->device, internalState->resource, &memRequirements );
        // 
        // VkMemoryAllocateInfo allocInfo = {};
        // allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        // allocInfo.allocationSize = memRequirements.size;
        // allocInfo.memoryTypeIndex = FindMemoryType( (VkPhysicalDevice)VulkanPhysicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
        // 
        // if ( vkAllocateMemory( internalState->device, &allocInfo, nullptr, &internalState->memory ) != VK_SUCCESS )
        // {
        //     EE_LOG_CORE_CRITICAL( L"Failed to allocate image memory!" );
        //     return false;
        // }
        // 
        // vkBindImageMemory( internalState->device, internalState->resource, internalState->memory, 0 );
        
        outTexture.type = RenderingResourceType_Texture;
        outTexture.internalState = internalState;
        outTexture.description = description;

        return true;
    }

    bool VulkanRHI::CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const { return false; }

    bool VulkanRHI::CreateShaderStage( EShaderStage stage, const void* code, size_t codeLength, ShaderStage& outShaderStage ) const
    {
        VulkanRenderDevice* internalDevice = ToInternal( device, RenderDevice );
        auto internalState = std::make_shared<VulkanShaderStage>();
        internalState->device = internalDevice->device;

        VkShaderModuleCreateInfo createInfo
        {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, // sType
            NULL,                                        // pNext
            0,                                           // flags
            codeLength,                                  // codeSize
            reinterpret_cast<const uint32_t*>( code ),   // pCode
        };
        
        if ( vkCreateShaderModule( internalState->device, &createInfo, nullptr, &internalState->shaderModule ) != VK_SUCCESS )
        {
            EE_LOG_CORE_ERROR( "Failed to create shader module!" );
            return false;
        }
        
        VkPipelineShaderStageCreateInfo vertShaderStageInfo
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,    // sType;
            VK_NULL_HANDLE,                                         // pNext;
            0,                                                      // flags;
            Vulkan_ConvertShaderStage( stage ),                     // stage;
            internalState->shaderModule,                            // module;
            "main",                                                 // pName;
            NULL                                                    // pSpecializationInfo;
        };

        outShaderStage.type = RenderingResourceType_Shader;
        outShaderStage.stage = stage;
        outShaderStage.internalState = internalState;

        return true;
    }

    bool VulkanRHI::CreateBuffer( const BufferDescription& description, Buffer& outBuffer ) const
    {
        VulkanRenderDevice* internalDevice = ToInternal( device, RenderDevice );
        auto internalState = std::make_shared<VulkanBuffer>();
        internalState->device = internalDevice->device;

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.sharingMode = Vulkan_ConvertSharingMode( description.sharing );
        bufferInfo.usage = Vulkan_ConvertBufferUsageFlags( description.usages );
        bufferInfo.size = description.size;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        if ( vmaCreateBuffer( internalDevice->allocator, &bufferInfo, &allocInfo, &internalState->buffer, &internalState->nativeAllocation, VK_NULL_HANDLE ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to allocate buffer memory!" );
            return false;
        }

        outBuffer.type = RenderingResourceType_Buffer;
        outBuffer.description = description;
        outBuffer.internalState = internalState;
        return true;
    }

	void VulkanRHI::SetName( RenderResource* pResource, const NChar* name ) { }

	EE::CommandList VulkanRHI::BeginCommandList( ECommandQueueType queue ) { return CommandList(); }

	void VulkanRHI::SubmitCommandLists() { }

	void VulkanRHI::WaitForDevice() const { }

	Texture* VulkanRHI::GetBackBuffer( const SwapChain& swapchain ) const
    {
        auto swapchainInternal = ToInternal( swapchain, SwapChain );
        return &swapchainInternal->imageTextures[ 0 ];
    }

	void VulkanRHI::RenderPassBegin( const SwapChain& swapchain, CommandList cmd )
	{
		auto internalSwapChain = ToInternal( swapchain, SwapChain );
		// Vector4 clearColor = swapchain.description.clearcolor * 255;
        // 
        // SDL_Texture* buffer = ToInternal( *internalSwapChain->backBuffers[ 0 ] )->resource;
        // 
        // SDL_SetRenderTarget( internalSwapChain->renderer, buffer );
        // 
        // const auto bm = SDL_ComposeCustomBlendMode(
        //     SDL_BLENDFACTOR_ONE,
        //     SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        //     SDL_BLENDOPERATION_ADD,
        // 
        //     SDL_BLENDFACTOR_ONE,
        //     SDL_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        //     SDL_BLENDOPERATION_ADD );
        // SDL_SetTextureBlendMode( buffer, bm );
        // 
        // SDL_SetRenderDrawBlendMode( internalSwapChain->renderer, SDL_BLENDMODE_NONE );
        // SDL_SetRenderDrawColor( internalSwapChain->renderer,
        // 	(Uint8)clearColor.r, (Uint8)clearColor.g, (Uint8)clearColor.b, (Uint8)clearColor.a );
        // SDL_RenderFillRect( internalSwapChain->renderer, NULL );
        // 
        // SDL_RenderTexture( internalSwapChain->renderer, buffer, NULL, NULL );

		// SDL_RenderClear( internalSwapChain->renderer );
        // 
        // SDL_SetRenderDrawBlendMode( internalSwapChain->renderer, SDL_BLENDMODE_ADD );
        // SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_MOD );
        // SDL_RenderTexture( internalSwapChain->renderer, texture, NULL, NULL);
	}

	void VulkanRHI::RenderPassEnd( const SwapChain& swapchain, CommandList cmd )
	{
		auto internalSwapChain = ToInternal( swapchain, SwapChain );
		// SDL_RenderPresent( internalSwapChain->renderer );
	}

	void VulkanRHI::BindScissorRects( uint32 numRects, const Rect* rects, CommandList cmd ) { }

	void VulkanRHI::BindResource( EShaderStage stage, const RenderResource* resource, uint32 slot, CommandList cmd, int subresource ) { }

	void VulkanRHI::BindResources( EShaderStage stage, const RenderResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }

	void VulkanRHI::BindUAV( EShaderStage stage, const RenderResource* resource, uint32 slot, CommandList cmd, int subresource ) { }

	void VulkanRHI::BindUAVs( EShaderStage stage, const RenderResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }

	void VulkanRHI::UnbindResources( uint32 slot, uint32 num, CommandList cmd ) { }

	void VulkanRHI::UnbindUAVs( uint32 slot, uint32 num, CommandList cmd ) { }

	void VulkanRHI::BindSampler( EShaderStage stage, const Sampler* sampler, uint32 slot, CommandList cmd ) { }

	void VulkanRHI::BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32 slot, CommandList cmd ) { }

	void VulkanRHI::BindVertexBuffers( const Buffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, CommandList cmd ) { }

	void VulkanRHI::BindIndexBuffer( const Buffer* indexBuffer, uint32 offset, CommandList cmd ) { }

	void VulkanRHI::BindStencilRef( uint32 value, CommandList cmd ) { }

	void VulkanRHI::BindBlendFactor( float r, float g, float b, float a, CommandList cmd ) { }

	void VulkanRHI::Draw( uint32 vertexCount, uint32 startVertexLocation, CommandList cmd ) { }

	void VulkanRHI::DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation, CommandList cmd ) { }

	void VulkanRHI::DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }

	void VulkanRHI::DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }

	void VulkanRHI::DrawInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }

	void VulkanRHI::DrawIndexedInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }
	
	void VulkanRHI::Dispatch( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) { }

	void VulkanRHI::DispatchIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }

	void VulkanRHI::CopyResource( const RenderResource* pDst, const RenderResource* pSrc, CommandList cmd ) { }

    void VulkanRHI::UpdateTexture( const Texture& texture, const void* data )
    {
        auto textureInternal = ToInternal( texture, Texture );

        // SDL_UpdateTexture(
        //     textureInternal->resource,
        //     NULL, data, (int)(texture.description.width * sizeof( uint32 ))
        // );
    }

	void VulkanRHI::EventBegin( const char* name, CommandList cmd ) { }

	void VulkanRHI::EventEnd( CommandList cmd ) { }

	void VulkanRHI::SetMarker( const char* name, CommandList cmd ) { }

    WString const& VulkanRHI::GetName() const
    {
        static WString const deviceName = L"Vulkan " + Text::FormatUnit( VK_API_VERSION_MAJOR(VK_API_VERSION_1_3), 0 ) + L'.' + Text::FormatUnit( VK_API_VERSION_MINOR(VK_API_VERSION_1_3), 0 );
        return deviceName;
    }
}