
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"
#include "Engine/Video.h"

#include "RHI/Vulkan/VulkanRHI.h"

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

#define GetVulkanInternal( x, type ) static_cast<Vulkan##type*>( x.internalState.get() )

namespace EE
{
    static VkShaderStageFlagBits ConvertShaderStage( EShaderStage shaderStage )
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

    static VkFormat ConvertPixelFormat( EPixelFormat format, EColorSpace colorspace )
    {
        switch ( format )
        {
        case PixelFormat_R8G8_UNORM:
            if ( colorspace == ColorSpace_Linear ) { return VK_FORMAT_R8G8_UNORM; }
            return VK_FORMAT_R8G8_SRGB;
        case PixelFormat_R16G16B16A16_SNORM:               return VK_FORMAT_R16G16B16A16_SFLOAT;
        case PixelFormat_A2R10G10B10_UNORM:                return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case PixelFormat_R8G8B8A8_UNORM:
            if ( colorspace == ColorSpace_Linear ) { return VK_FORMAT_B8G8R8A8_UNORM; }
            return VK_FORMAT_B8G8R8A8_SRGB;
        case PixelFormat_DepthComponent24:                 return VK_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat_DepthStencil:                     return VK_FORMAT_D16_UNORM_S8_UINT;
        case PixelFormat_ShadowDepth:                      return VK_FORMAT_D16_UNORM;
        default:
            return VK_FORMAT_UNDEFINED;
        }
    }

    static EPixelFormat ConvertPixelFormat( VkFormat vkFormat )
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

    static EColorSpace ConvertColorSpace( VkColorSpaceKHR colorspace )
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

    static bool FrameColorSpaceIsHDR( VkColorSpaceKHR colorspace )
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

    VkImageTiling ConvertTextureTiling( ETilingMode tiling )
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

    VkImageViewType ConvertTextureType( ETextureType type )
    {
        switch ( type )
        {
        case TextureType_Texture1D:     return VK_IMAGE_VIEW_TYPE_1D;
        case TextureType_Texture2D:     return VK_IMAGE_VIEW_TYPE_2D;
        case TextureType_Texture3D:     return VK_IMAGE_VIEW_TYPE_3D;
        case TextureType_Cubemap:       return VK_IMAGE_VIEW_TYPE_CUBE;
        default:
            return VK_IMAGE_VIEW_TYPE_2D;
        }
    }

    VkImageUsageFlags ConvertTextureUsage( EUsageMode usage )
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

    VkSharingMode ConvertSharingMode( ESharingMode sharing )
    {
        switch ( sharing )
        {
        case SharingMode_Dynamic:       return VK_SHARING_MODE_CONCURRENT;
        case SharingMode_Static:        return VK_SHARING_MODE_EXCLUSIVE;
        case SharingMode_Default:       return VK_SHARING_MODE_EXCLUSIVE;
        default:
            return VK_SHARING_MODE_EXCLUSIVE;
        }
    }

    VkBufferUsageFlags ConvertBufferUsageFlags( EBufferUsageFlags usages )
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

    struct VulkanRHIInstance;
    struct VulkanRHIPhysicalDevice;
    struct VulkanRHIDevice;
    struct VulkanRHICommandPool;
    struct VulkanRHICommandBuffer;
    struct VulkanRHIPresentContext;
    struct VulkanRHIBuffer;
    struct VulkanRHITexture;
    struct VulkanRHISwapChain;
    struct VulkanRHISurface;
    struct VulkanRHIShaderStage;

    struct VulkanRHICommandPool
    {
        VkDevice device;
        uint32 queueFamilyIndex;
        VkCommandPool commandPool;

        ~VulkanRHICommandPool()
        {
            if ( commandPool != VK_NULL_HANDLE )
            {
                vkDestroyCommandPool( device, commandPool, VK_NULL_HANDLE );
            }
        }
    };

    struct VulkanCommandBuffer
    {
        VkDevice device;
        uint32 queueFamilyIndex;
        VkCommandBuffer commandBuffer;
    };

    struct VulkanSurfaceSupportDetails
    {
        VkBool32 supported = false;
        VkSurfaceCapabilitiesKHR capabilities = {};
        uint32 formatCount = 0;
        TArray<VkSurfaceFormatKHR> formats;
        uint32 presentModeCount = 0;
        TArray<VkPresentModeKHR> presentModes;
    };

    struct VulkanRHIPhysicalDevice
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties deviceProperties{};
        VkPhysicalDeviceFeatures deviceFeatures{};
        uint32 extensionCount = 0;
        TArray<VkExtensionProperties> extensions;
        uint32 queueFamilyCount = 0;
        TArray<VkQueueFamilyProperties> queueFamilies;
        TMap<VkSurfaceKHR, VulkanSurfaceSupportDetails> surfaceSupportDetails;
    };

    struct VulkanRHIInstance
    {
        VkInstance instance = VK_NULL_HANDLE;

        uint32 physicalDeviceCount = 0;
        TArray<VulkanRHIPhysicalDevice> physicalDevices;
        uint32 selectedDeviceIndex = 0;

        ~VulkanRHIInstance()
        {
            vkDestroyInstance( instance, nullptr );
        }
    };

    struct VulkanRHIDevice
    {
        uint32 physicalDeviceIndex = 0;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VmaAllocator allocator = VK_NULL_HANDLE;
        // map of command pools by queue family index
        TMap<uint32, VulkanRHICommandPool*> commandPools;

        union
        {
            uint32 graphicsQueueIndex;
            uint32 presentQueueIndex;
            uint32 queueFamilyIndices[ 2 ]{};
        };

        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        ~VulkanRHIDevice()
        {
            for ( auto& pair : commandPools )
                delete pair.second;
            vmaDestroyAllocator( allocator );
            vkDestroyDevice( device, nullptr );
        }
    };

    struct VulkanRHIPresentContext
    {
    };

    struct VulkanRHIBuffer : public RHIBuffer
    {
        VkDevice device = VK_NULL_HANDLE;
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation nativeAllocation = VK_NULL_HANDLE;
        EBufferUsageFlags usages = BufferUsage_None;

        ~VulkanRHIBuffer()
        {
            if ( buffer != NULL )
            {
                vkDestroyBuffer( device, buffer, NULL );
            }
        }
    };

    struct VulkanRHITexture
    {
        VkDevice device = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImage resource = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VmaAllocation memory = VK_NULL_HANDLE;

        ~VulkanRHITexture()
        {
            if ( resource != NULL )
            {
                vkDestroyImage( device, resource, NULL );
            }

            vkDestroyImageView( device, imageView, NULL );
        }
    };

    struct VulkanRHISwapChain
    {
        VkDevice device = VK_NULL_HANDLE;
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;
        VkExtent2D size = {};
        uint32 imageCount = 0;
        TArray<VkImage> images;
        TArray<RHITexture> imageTextures;

        ~VulkanRHISwapChain()
        {
            for ( uint32 i = 0; i < imageCount; i++ )
            {
                images[ i ] = NULL;
                GetVulkanInternal( imageTextures[ i ], RHITexture )->resource = NULL;
            }

            vkDestroySwapchainKHR( device, swapChain, NULL );
        }
    };

    struct VulkanRHISurface
    {
        VkInstance instance = VK_NULL_HANDLE;
        VulkanSurfaceSupportDetails details;
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        ~VulkanRHISurface()
        {
            vkDestroySurfaceKHR( instance, surface, nullptr);
        }
    };

    struct VulkanRHIShaderStage
    {
        VkDevice device = VK_NULL_HANDLE;
        VkShaderModule shaderModule = VK_NULL_HANDLE;
        
        ~VulkanRHIShaderStage()
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

    VulkanRHI::~VulkanRHI()
    {
        if ( initialized )
        {
            device.internalState.reset();
            instance.internalState.reset();

            SDL_Vulkan_UnloadLibrary();
        }
    }

    EDynamicRHI VulkanRHI::GetType() const
    {
        return EDynamicRHI::Vulkan;
    }

    bool GetPhysicalDeviceSurfaceCapabilities( VulkanRHIInstance* instance, VulkanRHISurface* surface )
    {
        for ( uint32 i = 0; i < instance->physicalDeviceCount; i++ )
        {
            auto& internalPhysicalDevice = instance->physicalDevices[ i ];
            auto& surfaceSupportDetails = internalPhysicalDevice.surfaceSupportDetails;
            
            if ( auto search = surfaceSupportDetails.find( surface->surface ); search != surfaceSupportDetails.end() )
                continue;
            
            VulkanSurfaceSupportDetails internalSurfaceDetails;
            vkGetPhysicalDeviceSurfaceSupportKHR( internalPhysicalDevice.physicalDevice, i, surface->surface, &internalSurfaceDetails.supported );

            if ( internalSurfaceDetails.supported == VK_FALSE )
            {
                surface->details = internalSurfaceDetails;
                surfaceSupportDetails.insert( std::make_pair( surface->surface, internalSurfaceDetails ) );
                continue;
            }

            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( internalPhysicalDevice.physicalDevice, surface->surface, &internalSurfaceDetails.capabilities );

            vkGetPhysicalDeviceSurfaceFormatsKHR( internalPhysicalDevice.physicalDevice, surface->surface,
                &internalSurfaceDetails.formatCount, VK_NULL_HANDLE );
            if ( internalSurfaceDetails.formatCount != 0 )
            {
                internalSurfaceDetails.formats.resize( internalSurfaceDetails.formatCount );
                vkGetPhysicalDeviceSurfaceFormatsKHR( internalPhysicalDevice.physicalDevice, surface->surface,
                    &internalSurfaceDetails.formatCount, internalSurfaceDetails.formats.data() );
            }

            vkGetPhysicalDeviceSurfacePresentModesKHR( internalPhysicalDevice.physicalDevice, surface->surface,
                &internalSurfaceDetails.presentModeCount, VK_NULL_HANDLE );
            if ( internalSurfaceDetails.presentModeCount != 0 )
            {
                internalSurfaceDetails.presentModes.resize( internalSurfaceDetails.presentModeCount );
                vkGetPhysicalDeviceSurfacePresentModesKHR( internalPhysicalDevice.physicalDevice, surface->surface,
                    &internalSurfaceDetails.presentModeCount, internalSurfaceDetails.presentModes.data() );
            }

            surface->details = internalSurfaceDetails;
            surfaceSupportDetails.insert( std::make_pair( surface->surface, internalSurfaceDetails ) );
        }

        return true;
    }

    bool IsDeviceSuitable( VulkanRHIPhysicalDevice* physicalDevice, VulkanRHISurface* surface );

    int RateDeviceSuitabilityForSurface( VulkanRHIPhysicalDevice* physicalDevice, VulkanRHISurface* surface )
    {
        int score = 0;

        // Discrete GPUs have a significant performance advantage
        if ( physicalDevice->deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
        {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += physicalDevice->deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if ( physicalDevice->deviceFeatures.geometryShader == VK_FALSE )
        {
            return 0;
        }

        if ( IsDeviceSuitable( physicalDevice, surface ) == false )
        {
            return 0;
        }

        EE_LOG_CORE_INFO(
            L"\u2570\u2500> {0} : {1}",
            Text::NarrowToWide( physicalDevice->deviceProperties.deviceName ),
            score
        );

        return score;
    }

    int32 PickPhysicalDeviceForSurface( VulkanRHIInstance* instance, VulkanRHISurface* surface )
    {
        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, int> candidates;

        for ( uint32 i = 0; i < instance->physicalDeviceCount; i++ )
        {
            int score = RateDeviceSuitabilityForSurface( &instance->physicalDevices[ i ], surface );
            candidates.insert( std::make_pair( score, i ) );
        }

        // Check if the best candidate is suitable at all
        if ( candidates.rbegin()->first > 0 )
        {
            return candidates.rbegin()->second;
        }
        else
        {
            return -1;
        }
    }
    
    QueueFamilyIndices GetQueueFamilies( VulkanRHIPhysicalDevice* physicalDevice )
    {
        QueueFamilyIndices indices;

        int i = 0;
        for ( const auto& queueFamily : physicalDevice->queueFamilies )
        {
            if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT )
            {
                indices.graphicsFamily = i;
            }

            // Vulkan tutorial asks for surface support here, 
            // this is validated before selecting device, so Im ignoring that
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


    const TArray<const NChar*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const float QueuePriorities[] = { 1.0F };
    
    bool CheckDeviceExtensionSupport( VulkanRHIPhysicalDevice* physicalDevice )
    {
        std::set<std::string> requiredExtensions( DeviceExtensions.begin(), DeviceExtensions.end() );

        for ( const auto& extension : physicalDevice->extensions )
        {
            requiredExtensions.erase( extension.extensionName );
        }

        return requiredExtensions.empty();
    }
    
    bool IsDeviceSuitable( VulkanRHIPhysicalDevice* physicalDevice, VulkanRHISurface* surface )
    {
        QueueFamilyIndices indices = GetQueueFamilies( physicalDevice );
        bool extensionsSupported = CheckDeviceExtensionSupport( physicalDevice );

        bool swapChainAdequate = false;
        if ( auto search = physicalDevice->surfaceSupportDetails.find( surface->surface ); search != physicalDevice->surfaceSupportDetails.end() )
        {
            if ( search->second.supported )
            {
                swapChainAdequate = !surface->details.formats.empty() && !surface->details.presentModes.empty();
            }
        }

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

    bool SelectSuitableDevice( VulkanRHIInstance* instance, VulkanRHISurface* surface )
    {
        int selection = PickPhysicalDeviceForSurface( instance, surface );

        if ( selection < 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find a suitable device!" );
            return false;
        }

        instance->selectedDeviceIndex = selection;

        return true;
    }

    bool CreateCommandBufferPools( VulkanRHIDevice* device )
    {
        std::set<uint32> uniqueQueueFamilies = { device->graphicsQueueIndex, device->physicalDeviceIndex };

        for ( uint32 queueFamily : uniqueQueueFamilies )
        {
            VkCommandPoolCreateInfo createInfo
            {
                VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, // sType;
                VK_NULL_HANDLE,                             // pNext;
                0,                                          // flags;
                queueFamily                                 // queueFamilyIndex;
            };

            auto commandPool = new VulkanRHICommandPool();
            commandPool->device = device->device;
            commandPool->queueFamilyIndex = queueFamily;

            auto createResult = vkCreateCommandPool( device->device, &createInfo, NULL, &commandPool->commandPool );
            if ( createResult != VK_SUCCESS )
            {
                EE_LOG_CORE_CRITICAL( L"Failed to create command pool: {0}", (int32)createResult );
                return false;
            }

            device->commandPools.insert( std::make_pair( queueFamily, commandPool ) );
        }

        return true;
    }

    bool VulkanRHI::Initialize()
    {
        if ( initialized )
            return false;
        
        auto internalState = std::make_shared<VulkanRHIInstance>();

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
        if ( (createResult = vkCreateInstance( &createInfo, VK_NULL_HANDLE, &internalState->instance )) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"vkCreateInstance failed : {0}", (int32)createResult );
            return false;
        }

        // Get physical devices
        vkEnumeratePhysicalDevices( internalState->instance, &internalState->physicalDeviceCount, VK_NULL_HANDLE );
        if ( internalState->physicalDeviceCount == 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find GPUs with Vulkan support!" );
            return false;
        }

        EE_LOG_CORE_INFO( L"\u250C> Available devices : {0}", internalState->physicalDeviceCount );

        // Get physical device info
        internalState->physicalDevices.resize( internalState->physicalDeviceCount );
        TArray<VkPhysicalDevice> physicalDevices( internalState->physicalDeviceCount );
        vkEnumeratePhysicalDevices( internalState->instance, &internalState->physicalDeviceCount, physicalDevices.data() );

        for ( uint32 i = 0; i < internalState->physicalDeviceCount; i++ )
        {
            auto& physicalDevice = physicalDevices[ i ];
            auto& internalPhysicalDevice = internalState->physicalDevices[ i ];
            internalPhysicalDevice.physicalDevice = physicalDevice; 
            vkGetPhysicalDeviceFeatures( physicalDevice, &internalPhysicalDevice.deviceFeatures );
            vkGetPhysicalDeviceProperties( physicalDevice, &internalPhysicalDevice.deviceProperties );

            // Logic to find queue family indices to populate struct with
            vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &internalPhysicalDevice.queueFamilyCount, VK_NULL_HANDLE );
            internalPhysicalDevice.queueFamilies.resize( internalPhysicalDevice.queueFamilyCount );
            vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &internalPhysicalDevice.queueFamilyCount, internalPhysicalDevice.queueFamilies.data() );

            // Get extension support
            vkEnumerateDeviceExtensionProperties( physicalDevice, VK_NULL_HANDLE, &internalPhysicalDevice.extensionCount, VK_NULL_HANDLE );
            internalPhysicalDevice.extensions.resize( internalPhysicalDevice.extensionCount );
            vkEnumerateDeviceExtensionProperties( physicalDevice, VK_NULL_HANDLE, &internalPhysicalDevice.extensionCount, internalPhysicalDevice.extensions.data() );
        }

        instance.internalState = internalState;
        return initialized = true;
    }

    bool VulkanRHI::CreateDevice( RHISurface& surface, RHIDevice& outDevice ) const
    {
        VulkanRHIInstance* internalInstance = GetVulkanInternal( instance, RHIInstance );
        auto internalState = std::make_shared<VulkanRHIDevice>();
        VulkanRHIPhysicalDevice* physicalDevice = &internalInstance->physicalDevices[ internalInstance->selectedDeviceIndex ];
        internalState->physicalDevice = physicalDevice->physicalDevice;
        internalState->physicalDeviceIndex = internalInstance->selectedDeviceIndex;

        // Specifying the queues to be created
        QueueFamilyIndices indices = GetQueueFamilies( physicalDevice );

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
        deviceFeatures.geometryShader = VK_TRUE;
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo deviceCreateInfo
        {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,           // sType
            NULL,                                           // pNext
            0,                                              // flags
            static_cast<uint32>(queueCreateInfos.size()),   // queueCreateInfoCount
            queueCreateInfos.data(),                        // pQueueCreateInfos
            0,                                              // enabledLayerCount    deprecated?
            NULL,                                           // ppEnabledLayerNames  deprecated?
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
            CreateNativeVmaAllocator( GetVulkanInternal( instance, RHIInstance )->instance, internalState->physicalDevice, internalState->device, &internalState->allocator ) == VK_SUCCESS,
            L"Failed to create VMA Allocator!"
        );

        vkGetDeviceQueue( internalState->device, internalState->graphicsQueueIndex = indices.graphicsFamily.value(), 0, &internalState->graphicsQueue );
        vkGetDeviceQueue( internalState->device, internalState->presentQueueIndex  = indices.presentFamily.value(),  0, &internalState->presentQueue  );
        
        if ( CreateCommandBufferPools( internalState.get() ) == false )
        {
            return false;
        }

        outDevice.internalState = internalState;
        return true;
    }

    bool VulkanRHI::CreatePresentContext( Window* window, RHIPresentContext& outContext )
    {
        auto internalState = std::make_shared<VulkanRHIPresentContext>();

        SurfaceDescription surfaceDescription{};
        surfaceDescription.window = window;
        if ( CreateSurface( surfaceDescription, outContext.surface ) == false )
        {
            return false;
        }

        // Vulkan needs the surface information in order to determine the driver, so the driver creation is here, the device selection is handled inside CreateSurface
        if ( device.internalState == NULL )
        {
            if ( SelectSuitableDevice( GetVulkanInternal( instance, RHIInstance ), GetVulkanInternal( outContext.surface, RHISurface ) ) == false )
            {
                return false;
            }

            if ( CreateDevice( outContext.surface, device ) == false )
            {
                return false;
            }
        }

        VulkanRHISurface* surface = GetVulkanInternal( outContext.surface, RHISurface );
        
        bool tryHDR = false;
        if ( window->GetAllowHDR() && SDL_GetBooleanProperty( SDL_GetDisplayProperties( SDL_GetPrimaryDisplay() ), SDL_PROP_DISPLAY_HDR_ENABLED_BOOLEAN, SDL_FALSE ) )
        {
            tryHDR = true;
        }

        EPixelFormat desiredFormat = PixelFormat_R8G8B8A8_UNORM;
        EColorSpace desiredColorSpace = ColorSpace_sRGB;
        // Find best format
        bool contains = false;
        for ( uint32 i = 0; i < surface->details.formatCount; i++ )
        {
            VkSurfaceFormatKHR& surfaceFormat = surface->details.formats[ i ];
            bool colorSpaceHDR = FrameColorSpaceIsHDR( surfaceFormat.colorSpace );
            EColorSpace colorSpace = ConvertColorSpace( surfaceFormat.colorSpace );
            EPixelFormat format = ConvertPixelFormat( surfaceFormat.format );

            if ( colorSpace == ColorSpace_Unknown )
                continue;
            if ( format == PixelFormat_Unknown )
                continue;

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
                if ( colorSpaceHDR == false )
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
        VulkanRHISwapChain* swapChain = GetVulkanInternal( outContext.swapChain, RHISwapChain );

        outContext.internalState = internalState;
        return true;
    }

    bool VulkanRHI::CreateSurface( const SurfaceDescription& description, RHISurface& outSurface ) const
    {
        auto internalState = std::make_shared<VulkanRHISurface>();
        internalState->surface = VkSurfaceKHR();
        internalState->instance = GetVulkanInternal( instance, RHIInstance )->instance;

        if ( SDL_Vulkan_CreateSurface( (SDL_Window*)description.window->GetWindowHandle(), internalState->instance, VK_NULL_HANDLE, &internalState->surface ) == SDL_FALSE )
        {
            EE_LOG_CORE_CRITICAL( L"SDL_Vulkan_CreateSurface failed! {0}", Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }
        
        GetPhysicalDeviceSurfaceCapabilities( GetVulkanInternal( instance, RHIInstance ), internalState.get() );

        description.window;
        outSurface.type = RenderingResourceType_Surface;
        outSurface.internalState = internalState;

        return true;
    }

    bool CreateImageView( VkDevice device, const TextureDescription& description, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageView* outImageView )
    {
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = ConvertTextureType( description.type );
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
    bool VulkanRHI::CreateSwapChain( const SwapChainDescription& description, Window* window, RHISwapChain& outSwapChain ) const
    {
        VulkanRHIDevice* internalDevice = GetVulkanInternal( device, RHIDevice );
        auto internalState = std::make_shared<VulkanRHISwapChain>();
        internalState->device = internalDevice->device;

        auto& presentContext = GetPresentContextOfWindow( window );
        auto surface = GetVulkanInternal( presentContext.surface, RHISurface );

        VkSurfaceFormatKHR desiredFormat;

        bool contains = false;
        for ( uint32 i = 0; i < surface->details.formatCount; i++ )
        {
            VkSurfaceFormatKHR& surfaceFormat = surface->details.formats[ i ];
            if ( ConvertPixelFormat( surfaceFormat.format ) == description.format )
            {
                if ( ConvertColorSpace( surfaceFormat.colorSpace ) == description.colorSpace )
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
        width = CLAMP( (uint32)width, surface->details.capabilities.minImageExtent.width, surface->details.capabilities.maxImageExtent.width );
        height = CLAMP( (uint32)height, surface->details.capabilities.minImageExtent.height, surface->details.capabilities.maxImageExtent.height );
        internalState->size.width = width;
        internalState->size.height = height;

        uint32 imageCount;
        imageCount = Math::Max( surface->details.capabilities.minImageCount + 1, description.bufferCount );
        imageCount = Math::Min( surface->details.capabilities.maxImageCount, description.bufferCount );

        VkSwapchainCreateInfoKHR createInfo = 
        {
            VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,                            // sType
            VK_NULL_HANDLE,                                                         // pNext
            0,                                                                      // flags
            surface->surface,                                                       // surface
            imageCount,                                                             // minImageCount
            desiredFormat.format,                                                   // imageFormat
            desiredFormat.colorSpace,                                               // imageColorSpace
            internalState->size,                                                    // imageExtent
            1,                                                                      // imageArrayLayers
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,  // imageUsage
        };

        if ( internalDevice->graphicsQueueIndex != internalDevice->presentQueueIndex )
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = internalDevice->queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 1;
            createInfo.pQueueFamilyIndices = internalDevice->queueFamilyIndices;
        }

        createInfo.preTransform = surface->details.capabilities.currentTransform;
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
            auto textureInternalState = std::make_shared<VulkanRHITexture>();
            textureInternalState->device = internalState->device;
            textureInternalState->resource = internalState->images[ i ];

            RHITexture& texture = internalState->imageTextures[ i ];
            TextureDescription description{};
            description.type = TextureType_Texture2D;
            description.format = description.format;
            description.colorSpace = description.colorSpace;
            description.height = height;
            description.width = width;
            description.arraySize = 1;
            description.mipLevels = 1;

            CreateImageView( internalState->device, description, internalState->images[ i ], desiredFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &textureInternalState->imageView );

            texture.type = RenderingResourceType_Texture;
            texture.internalState = textureInternalState;
        }

        outSwapChain.type = RenderingResourceType_Swapchain;
        outSwapChain.internalState = internalState;

        return true;
	}
    
    bool VulkanRHI::CreateCommandBuffer( const CommandBufferDescription& description, RHICommandBuffer& commandBuffer ) const
    {
        VulkanRHIDevice* internalDevice = GetVulkanInternal( device, RHIDevice );
        auto internalState = std::make_shared<VulkanCommandBuffer>();
        internalState->device = internalDevice->device;
        internalState->queueFamilyIndex = internalDevice->graphicsQueueIndex;

        VkCommandBufferAllocateInfo allocateInfo
        {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,                                 // sType
            NULL,                                                                           // pNext
            internalDevice->commandPools[ internalState->queueFamilyIndex ]->commandPool,   // commandPool
            VK_COMMAND_BUFFER_LEVEL_PRIMARY,                                                // level
            1                                                                               // commandBufferCount
        };

        VkResult result = vkAllocateCommandBuffers( internalState->device, &allocateInfo, &internalState->commandBuffer );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Unable to allocate command buffer {}", (int32)result );
        }

        return true;
    }

	bool VulkanRHI::CreateTexture( const TextureDescription& description, RHITexture& outTexture ) const
    {
        VulkanRHIDevice* internalDevice = GetVulkanInternal( device, RHIDevice );
        auto internalState = std::make_shared<VulkanRHITexture>();
        internalState->device = internalDevice->device;

        VkImageCreateInfo imageInfo = {
            VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                                        // sType
            VK_NULL_HANDLE,                                                             // pNext
            0,                                                                          // flags
            VK_IMAGE_TYPE_2D,                                                           // imageType
            ConvertPixelFormat( description.format, description.colorSpace ),           // format
            VkExtent3D {                                                                // extent
                .width = description.width,
                .height = description.height,
                .depth = description.depth },
            description.mipLevels,                                                      // mipLevels
            description.arraySize,                                                      // arrayLayers
            (VkSampleCountFlagBits)description.sampleCount,                             // samples
            ConvertTextureTiling( description.tiling ),                                 // tiling
            ConvertTextureUsage( description.usage ),                                   // usage
            ConvertSharingMode( description.sharing ),                                  // sharingMode
            0,                                                                          // queueFamilyIndexCount
            NULL,                                                                       // pQueueFamilyIndices
            VK_IMAGE_LAYOUT_UNDEFINED,                                                  // initialLayout
        };

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
        
        outTexture.type = RenderingResourceType_Texture;
        outTexture.internalState = internalState;

        return true;
    }

    bool VulkanRHI::CreateSampler( const SamplerDescription& description, RHISampler& outSampler ) const { return false; }

    bool VulkanRHI::CreateShaderStage( EShaderStage stage, const void* code, size_t codeLength, RHIShaderStage& outShaderStage ) const
    {
        VulkanRHIDevice* internalDevice = GetVulkanInternal( device, RHIDevice );
        auto internalState = std::make_shared<VulkanRHIShaderStage>();
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
        
        VkPipelineShaderStageCreateInfo shaderStageInfo
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,    // sType;
            VK_NULL_HANDLE,                                         // pNext;
            0,                                                      // flags;
            ConvertShaderStage( stage ),                            // stage;
            internalState->shaderModule,                            // module;
            "main",                                                 // pName;
            NULL                                                    // pSpecializationInfo;
        };

        outShaderStage.type = RenderingResourceType_Shader;
        outShaderStage.stage = stage;
        outShaderStage.internalState = internalState;

        return true;
    }

    bool VulkanRHI::CreateBuffer( const BufferDescription& description, RHIBuffer& outBuffer ) const
    {
        VulkanRHIDevice* internalDevice = GetVulkanInternal( device, RHIDevice );
        auto internalState = std::make_shared<VulkanRHIBuffer>();
        internalState->device = internalDevice->device;

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.sharingMode = ConvertSharingMode( description.sharing );
        bufferInfo.usage = ConvertBufferUsageFlags( description.usages );
        bufferInfo.size = description.size;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        if ( vmaCreateBuffer( internalDevice->allocator, &bufferInfo, &allocInfo, &internalState->buffer, &internalState->nativeAllocation, VK_NULL_HANDLE ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to allocate buffer memory!" );
            return false;
        }

        outBuffer.type = RenderingResourceType_Buffer;
        outBuffer.internalState = internalState;
        return true;
    }

	void VulkanRHI::SetName( RHIResource* pResource, const NChar* name ) { }

	void VulkanRHI::WaitForDevice() const { }

    RHITexture* VulkanRHI::GetBackBuffer( const RHISwapChain& swapchain ) const
    {
        auto swapchainInternal = GetVulkanInternal( swapchain, RHISwapChain );
        return (RHITexture*)&swapchainInternal->imageTextures[0];
    }

    WString const& VulkanRHI::GetName() const
    {
        static WString const deviceName = L"Vulkan " + Text::FormatUnit( VK_API_VERSION_MAJOR( VK_API_VERSION_1_3 ), 0 ) + L'.' + Text::FormatUnit( VK_API_VERSION_MINOR( VK_API_VERSION_1_3 ), 0 );
        return deviceName;
    }

	// void VulkanRHI::BindScissorRects( uint32 numRects, const Rect* rects, CommandList cmd ) { }
    // 
	// void VulkanRHI::BindResource( EShaderStage stage, const RHIResource* resource, uint32 slot, CommandList cmd, int subresource ) { }
    // 
	// void VulkanRHI::BindResources( EShaderStage stage, const RHIResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }
    // 
	// void VulkanRHI::BindUAV( EShaderStage stage, const RHIResource* resource, uint32 slot, CommandList cmd, int subresource ) { }
    // 
	// void VulkanRHI::BindUAVs( EShaderStage stage, const RHIResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }
    // 
	// void VulkanRHI::UnbindResources( uint32 slot, uint32 num, CommandList cmd ) { }
    // 
	// void VulkanRHI::UnbindUAVs( uint32 slot, uint32 num, CommandList cmd ) { }
    // 
	// void VulkanRHI::BindSampler( EShaderStage stage, const RHISampler* sampler, uint32 slot, CommandList cmd ) { }
    // 
	// void VulkanRHI::BindConstantBuffer( EShaderStage stage, const RHIBuffer* buffer, uint32 slot, CommandList cmd ) { }
    // 
	// void VulkanRHI::BindVertexBuffers( const RHIBuffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, CommandList cmd ) { }
    // 
	// void VulkanRHI::BindIndexBuffer( constRHI Buffer* indexBuffer, uint32 offset, CommandList cmd ) { }
    // 
	// void VulkanRHI::BindStencilRef( uint32 value, CommandList cmd ) { }
    // 
	// void VulkanRHI::BindBlendFactor( float r, float g, float b, float a, CommandList cmd ) { }
    // 
	// void VulkanRHI::Draw( uint32 vertexCount, uint32 startVertexLocation, CommandList cmd ) { }
    // 
	// void VulkanRHI::DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation, CommandList cmd ) { }
    // 
	// void VulkanRHI::DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }
    // 
	// void VulkanRHI::DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }
    // 
	// void VulkanRHI::DrawInstancedIndirect( const RHIBuffer* args, uint32 args_offset, CommandList cmd ) { }
    // 
	// void VulkanRHI::DrawIndexedInstancedIndirect( const RHIBuffer* args, uint32 args_offset, CommandList cmd ) { }
	// 
	// void VulkanRHI::Dispatch( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) { }
    // 
	// void VulkanRHI::DispatchIndirect( const RHIBuffer* args, uint32 args_offset, CommandList cmd ) { }
    // 
	// void VulkanRHI::CopyResource( const RHIResource* pDst, const RHIResource* pSrc, CommandList cmd ) { }
    // 
    // void VulkanRHI::UpdateTexture( const RHITexture& texture, const void* data )
    // {
    //     auto textureInternal = GetVulkanInternal( texture, RHITexture );
    // 
    //     // SDL_UpdateTexture(
    //     //     textureInternal->resource,
    //     //     NULL, data, (int)(texture.description.width * sizeof( uint32 ))
    //     // );
    // }
    // 
	// void VulkanRHI::EventBegin( const char* name, CommandList cmd ) { }
    // 
	// void VulkanRHI::EventEnd( CommandList cmd ) { }
    // 
	// void VulkanRHI::SetMarker( const char* name, CommandList cmd ) { }
}