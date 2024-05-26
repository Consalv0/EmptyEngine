
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"
#include "Graphics/Vulkan/VulkanGraphicsDevice.h"

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

namespace EE
{
    VkFormat ConvertPixelFormat( EPixelFormat format )
    {
        switch ( format )
        {
        case EE::PixelFormat_Unknown:
            break;
        case EE::PixelFormat_R8:
            break;
        case EE::PixelFormat_R32F:
            break;
        case EE::PixelFormat_R8G8:
            return VK_FORMAT_R8G8_UNORM;
        case EE::PixelFormat_RG32F:
            break;
        case EE::PixelFormat_RG16F:
            break;
        case EE::PixelFormat_R8G8B8:
            return VK_FORMAT_R8G8B8_UNORM;
        case EE::PixelFormat_RGB32F:
            break;
        case EE::PixelFormat_RGB16F:
            break;
        case EE::PixelFormat_R8G8B8A8:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case EE::PixelFormat_RGBA16_UShort:
            break;
        case EE::PixelFormat_RGBA32F:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case EE::PixelFormat_DepthComponent24:
            return VK_FORMAT_D16_UNORM_S8_UINT;
            break;
        case EE::PixelFormat_DepthStencil:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
            break;
        case EE::PixelFormat_ShadowDepth:
            break;
        default:
            break;
        }

        return VK_FORMAT_B8G8R8A8_UNORM;
    }

    VkImageTiling ConvertTextureTiling( ETilingMode tiling )
    {
        switch ( tiling )
        {
        case EE::TilingMode_Default:
            return VK_IMAGE_TILING_OPTIMAL;
        case EE::TilingMode_Optimal:
            return VK_IMAGE_TILING_OPTIMAL;
        case EE::TilingMode_Linear:
            return VK_IMAGE_TILING_LINEAR;
        default:
            break;
        }

        return VK_IMAGE_TILING_OPTIMAL;
    }

    VkImageUsageFlags ConvertTextureUsage( EUsageMode usage )
    {
        switch ( usage )
        {
        case EE::UsageMode_Sampled:
            return VK_IMAGE_USAGE_SAMPLED_BIT;
        case EE::UsageMode_Storage:
            return VK_IMAGE_USAGE_STORAGE_BIT;
        case EE::UsageMode_Color:
            return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        case EE::UsageMode_DepthStencil:
            return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        default:
            return VK_IMAGE_USAGE_SAMPLED_BIT;
        }
    }

    VkSharingMode ConvertSharingMode( ESharingMode sharing )
    {
        switch ( sharing )
        {
        case EE::SharingMode_Dynamic:
            return VK_SHARING_MODE_CONCURRENT;
            break;
        case EE::SharingMode_Static:
            return VK_SHARING_MODE_EXCLUSIVE;
            break;
        case EE::SharingMode_Default:
        default:
            return VK_SHARING_MODE_EXCLUSIVE;
        }
    }

    struct VulkanWindowContext* ToInternal( const WindowContext& windowContext )
    {
        return static_cast<VulkanWindowContext*>(windowContext.internalState.get());
    }

    struct VulkanSurface* ToInternal( const WindowSurface& windowSurface )
    {
        return static_cast<VulkanSurface*>(windowSurface.internalState.get());
    }

    struct VulkanSwapChain* ToInternal( const SwapChain& swapChain )
    {
        return static_cast<VulkanSwapChain*>(swapChain.internalState.get());
    }

    struct VulkanTexture* ToInternal( const Texture& texture )
    {
        return static_cast<VulkanTexture*>(texture.internalState.get());
    }

    struct VulkanWindowContext
    {
        union
        {
            uint32 graphicsQueueIndex;
            uint32 presentQueueIndex;
            uint32 queueFamilyIndices[ 2 ];
        };

        VkQueue graphicsQueue;
        VkQueue presentQueue;
    };

    struct VulkanTexture
    {
        VkSampler sampler;
        VkDevice device;
        VkImage resource;
        VkImageView imageView;
        VkDeviceMemory memory;

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
        VkDevice device;
        VkSwapchainKHR swapChain;
        VkExtent2D size;
        uint32 imageCount;
        std::vector<VkImage> images;
        std::vector<Texture> imageTextures;

        ~VulkanSwapChain()
        {
            for ( uint32 i = 0; i < imageCount; i++ )
            {
                images[ i ] = NULL;
                ToInternal( imageTextures[ i ] )->resource = NULL;
            }

            vkDestroySwapchainKHR( device, swapChain, NULL );
        }
    };

    struct VulkanSurface
    {
        VkInstance instance;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        VkSurfaceFormatKHR surfaceFormat;
        VkSurfaceKHR surface;

        ~VulkanSurface()
        {
            vkDestroySurfaceKHR( instance, surface, nullptr);
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

        bool isDeviceComplete()
        {
            return graphicsFamily.has_value();
        }
    }; 
    
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        if ( Initialized )
        {
            vkDestroyDevice( *(VkDevice*)VulkanDevice, nullptr );
            vkDestroyInstance( *(VkInstance*)VulkanInstance, nullptr );
            SDL_Vulkan_UnloadLibrary();
        }
    }

    EWindowGraphicsAPI VulkanGraphicsDevice::GetWindowGraphicsAPI() const
    {
        return EWindowGraphicsAPI::WindowGraphicsAPI_Vulkan;
    }

    bool IsDeviceSuitable( VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface );

    int RateDeviceSuitability( VkPhysicalDevice physicalDevice )
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

        if ( IsDeviceSuitable( physicalDevice, NULL ) == false )
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

    VkPhysicalDevice PickPhysicalDevice( const std::vector<VkPhysicalDevice>& physicalDevices )
    {
        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, VkPhysicalDevice> candidates;

        for ( const auto& device : physicalDevices )
        {
            int score = RateDeviceSuitability( device );
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
            
            if ( windowSurface == NULL )
            {
                if ( indices.isDeviceComplete() )
                {
                    break;
                }
            }
            else
            {
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
        if ( windowSurface != NULL )
        {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( physicalDevice, windowSurface );
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        else
        {
            return indices.isDeviceComplete() && extensionsSupported;
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


    bool VulkanGraphicsDevice::Initialize()
    {
        if ( Initialized )
            return false;
        
        VulkanDevice = new VkDevice*(NULL);
        VulkanInstance = new VkInstance*(NULL);
        VulkanPhysicalDevice = NULL;
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
        const char* const* extensionNames = 0;
        extensionNames = SDL_Vulkan_GetInstanceExtensions( &extensionCount );

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
        if ( (createResult = vkCreateInstance( &createInfo, nullptr, (VkInstance*)VulkanInstance ) ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"vkCreateInstance failed : {0}", (int32)createResult );
            return false;
        }

        if ( StartDevices() == false )
        {
            return false;
        }

        return Initialized = true;
    }

    bool VulkanGraphicsDevice::StartDevices()
    {
        // Get physical device
        uint32 physicalDeviceCount;
        vkEnumeratePhysicalDevices( *(VkInstance*)VulkanInstance, &physicalDeviceCount, nullptr );
        if ( physicalDeviceCount == 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find GPUs with Vulkan support!" );
            return false;
        }

        EE_LOG_CORE_INFO( L"\u250C> Available devices : {0}", physicalDeviceCount );

        // Find suitable device
        std::vector<VkPhysicalDevice> physicalDevices( physicalDeviceCount );
        vkEnumeratePhysicalDevices( *(VkInstance*)VulkanInstance, &physicalDeviceCount, physicalDevices.data() );
        VulkanPhysicalDevice = PickPhysicalDevice( physicalDevices );
        if ( VulkanPhysicalDevice == NULL )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find a suitable device!" );
            return false;
        }

        return true;
    }

    bool VulkanGraphicsDevice::CreateWindowContext( Window* window, WindowContext& windowContext ) const
    {
        auto internalState = std::make_shared<VulkanWindowContext>();

        windowContext.internalState = internalState;

        WindowSurfaceDescription surfaceDescription;
        surfaceDescription.window = window;
        if ( CreateWindowSurface( surfaceDescription, windowContext.windowSurface ) == false )
        {
            return false;
        }

        SwapChainDescription swapChainDesc{};
        swapChainDesc.width = window->GetWidth();
        swapChainDesc.height = window->GetHeight();
        swapChainDesc.fullscreen = window->GetWindowMode();
        swapChainDesc.format = PixelFormat_RGBA32F;
        swapChainDesc.vsync = window->GetVSync();
        if ( CreateSwapChain( swapChainDesc, window, windowContext.swapChain ) == false )
        {
            return false;
        }
        
        return true;
    }

    bool VulkanGraphicsDevice::CreateWindowSurface( const WindowSurfaceDescription& description, WindowSurface& outWindowSurface ) const
    {
        auto internalState = std::make_shared<VulkanSurface>();
        internalState->surface = VkSurfaceKHR();
        internalState->instance = *(VkInstance*)VulkanInstance;
        if ( SDL_Vulkan_CreateSurface( (SDL_Window*)description.window->GetWindowHandle(), internalState->instance, VK_NULL_HANDLE, &internalState->surface ) == SDL_FALSE )
        {
            EE_LOG_CORE_CRITICAL( L"SDL_Vulkan_CreateSurface failed! {0}", Text::NarrowToWide( SDL_GetError() ) );
            return false;
        }

        if ( IsDeviceSuitable( (VkPhysicalDevice)VulkanPhysicalDevice, internalState->surface ) == false )
        {
            EE_LOG_CORE_CRITICAL( L"Current device is not suitable device for window!" );
            return false;
        }

        auto contextInternalState = ToInternal( description.window->GetWindowContext() );

        // Specifying the queues to be created
        QueueFamilyIndices indices = FindQueueFamilies( (VkPhysicalDevice)VulkanPhysicalDevice, internalState->surface );

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
        queueCreateInfo.queueFamilyIndex = contextInternalState->graphicsQueueIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures { };
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

        if ( vkCreateDevice( (VkPhysicalDevice)VulkanPhysicalDevice, &deviceCreateInfo, nullptr, (VkDevice*)VulkanDevice ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create logical device!" );
            return false;
        }

        vkGetDeviceQueue( *(VkDevice*)VulkanDevice, contextInternalState->graphicsQueueIndex = indices.graphicsFamily.value(), 0, &contextInternalState->graphicsQueue );
        vkGetDeviceQueue( *(VkDevice*)VulkanDevice,  contextInternalState->presentQueueIndex = indices.presentFamily.value(),  0, &contextInternalState->presentQueue  );

        description.window;
        outWindowSurface.type = GraphicsResourceType_Swapchain;
        outWindowSurface.internalState = internalState;
        outWindowSurface.description = description;

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
    bool VulkanGraphicsDevice::CreateSwapChain( const SwapChainDescription& description, Window* window, SwapChain& outSwapChain ) const
	{
		auto internalState = std::make_shared<VulkanSwapChain>();
        internalState->device = *(VkDevice*)VulkanDevice;
        auto surface = ToInternal( window->GetWindowContext().windowSurface );
        auto contextInternalState = ToInternal( window->GetWindowContext() );

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( (VkPhysicalDevice)VulkanPhysicalDevice, surface->surface, &surface->surfaceCapabilities );

        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        uint32 surfaceFormatsCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR( (VkPhysicalDevice)VulkanPhysicalDevice, surface->surface, &surfaceFormatsCount, NULL );
        surfaceFormats.resize( surfaceFormatsCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( (VkPhysicalDevice)VulkanPhysicalDevice, surface->surface, &surfaceFormatsCount, surfaceFormats.data() );

        VkSurfaceFormatKHR desiredFormat;
        desiredFormat.format = ConvertPixelFormat( description.format );
        bool contains = false;
        for ( uint32 i = 0; i < surfaceFormatsCount; i++ )
        {
            VkSurfaceFormatKHR& surfaceFormat = surfaceFormats[ i ];
            if ( surfaceFormat.format == desiredFormat.format )
            {
                if ( description.allow_hdr && surfaceFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR )
                {

                }
                contains = true;
            }
        }

        if ( contains == false )
        {
            EE_LOG_CORE_CRITICAL( L"Surface Format {0} is not supported!", (uint32)description.format );
            return false;
        }

        surface->surfaceFormat = desiredFormat;
        int width, height = 0;
        SDL_GetWindowSizeInPixels( (SDL_Window*)window->GetWindowHandle(), &width, &height);
        width = CLAMP( (uint32)width, surface->surfaceCapabilities.minImageExtent.width, surface->surfaceCapabilities.maxImageExtent.width );
        height = CLAMP( (uint32)height, surface->surfaceCapabilities.minImageExtent.height, surface->surfaceCapabilities.maxImageExtent.height );
        internalState->size.width = width;
        internalState->size.height = height;

        uint32 imageCount = surface->surfaceCapabilities.minImageCount + 1;
        if ( surface->surfaceCapabilities.maxImageCount > 0 && imageCount > surface->surfaceCapabilities.maxImageCount )
        {
            imageCount = surface->surfaceCapabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface->surface;
        createInfo.minImageCount = surface->surfaceCapabilities.minImageCount;
        createInfo.imageFormat = surface->surfaceFormat.format;
        createInfo.imageColorSpace = surface->surfaceFormat.colorSpace;
        createInfo.imageExtent = internalState->size;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if ( contextInternalState->graphicsQueueIndex != contextInternalState->presentQueueIndex )
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = contextInternalState->queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = surface->surfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        vkCreateSwapchainKHR( *(VkDevice*)VulkanDevice, &createInfo, NULL, &internalState->swapChain );

        vkGetSwapchainImagesKHR( *(VkDevice*)VulkanDevice, internalState->swapChain, &internalState->imageCount, NULL );
        internalState->images.resize( internalState->imageCount );
        vkGetSwapchainImagesKHR( *(VkDevice*)VulkanDevice, internalState->swapChain, &internalState->imageCount, internalState->images.data() );

        internalState->imageTextures.resize( internalState->imageCount );
        for ( uint32 i = 0; i < internalState->imageCount; i++ )
        {
            auto textureInternalState = std::make_shared<VulkanTexture>();
            textureInternalState->device = internalState->device;
            textureInternalState->resource = internalState->images[ i ];

            Texture& texture = internalState->imageTextures[ i ];
            texture.description.type = TextureType_Texture2D;
            texture.description.format = description.format;
            texture.description.height = height;
            texture.description.width = width;
            texture.description.arraySize = 1;
            texture.description.mipLevels = 1;

            CreateImageView( *(VkDevice*)VulkanDevice, texture.description, internalState->images[ i ], surface->surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, &textureInternalState->imageView );

            texture.type = GraphicsResourceType_Texture;
            texture.internalState = textureInternalState;
        }

        outSwapChain.type = GraphicsResourceType_Swapchain;
        outSwapChain.internalState = internalState;

        return true;
	}

	bool VulkanGraphicsDevice::CreateBuffer( const GPUBufferDescription& description, const SubresourceData* pInitialData, Buffer& outBuffer ) const { return false; }

	bool VulkanGraphicsDevice::CreateTexture( const TextureDescription& description, const SubresourceData* pInitialData, Texture& outTexture ) const 
    {
        auto internalState = std::make_shared<VulkanTexture>();
        internalState->device = *(VkDevice*)VulkanDevice;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = description.width;
        imageInfo.extent.height = description.height;
        imageInfo.extent.depth = description.depth;
        imageInfo.mipLevels = description.mipLevels;
        imageInfo.arrayLayers = description.arraySize;
        imageInfo.format = ConvertPixelFormat( description.format );
        imageInfo.tiling = ConvertTextureTiling( description.tiling );
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = ConvertTextureUsage( description.usage );
        imageInfo.samples = (VkSampleCountFlagBits)description.sampleCount;
        imageInfo.sharingMode = ConvertSharingMode( description.sharing );

        if ( vkCreateImage( internalState->device, &imageInfo, nullptr, &internalState->resource ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create image!" );
            return false;
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements( internalState->device, internalState->resource, &memRequirements );

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType( (VkPhysicalDevice)VulkanPhysicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );

        if ( vkAllocateMemory( internalState->device, &allocInfo, nullptr, &internalState->memory ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to allocate image memory!" );
            return false;
        }

        vkBindImageMemory( internalState->device, internalState->resource, internalState->memory, 0 );
        return true;
    }

	bool VulkanGraphicsDevice::CreateSampler( const SamplerDescription& description, Sampler& outSampler ) const { return false; }

	bool VulkanGraphicsDevice::CreateShader( EShaderStage stage, const void* pShaderBytecode, size_t BytecodeLength, Shader& outShader ) const { return false; }

	void VulkanGraphicsDevice::SetName( GraphicsDeviceResource* pResource, const NChar* name ) { }

	EE::CommandList VulkanGraphicsDevice::BeginCommandList( ECommandQueueType queue ) { return CommandList(); }

	void VulkanGraphicsDevice::SubmitCommandLists() { }

	void VulkanGraphicsDevice::WaitForDevice() const { }

	Texture* VulkanGraphicsDevice::GetBackBuffer( const SwapChain& swapchain ) const
    {
        auto swapchainInternal = ToInternal( swapchain );
        return &swapchainInternal->imageTextures[ 0 ];
    }

	void VulkanGraphicsDevice::RenderPassBegin( const SwapChain& swapchain, CommandList cmd )
	{
		auto internalSwapChain = ToInternal( swapchain );
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

	void VulkanGraphicsDevice::RenderPassEnd( const SwapChain& swapchain, CommandList cmd )
	{
		auto internalSwapChain = ToInternal( swapchain );
		// SDL_RenderPresent( internalSwapChain->renderer );
	}

	void VulkanGraphicsDevice::BindScissorRects( uint32 numRects, const Rect* rects, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindResource( EShaderStage stage, const GraphicsDeviceResource* resource, uint32 slot, CommandList cmd, int subresource ) { }

	void VulkanGraphicsDevice::BindResources( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindUAV( EShaderStage stage, const GraphicsDeviceResource* resource, uint32 slot, CommandList cmd, int subresource ) { }

	void VulkanGraphicsDevice::BindUAVs( EShaderStage stage, const GraphicsDeviceResource* const* resources, uint32 slot, uint32 count, CommandList cmd ) { }

	void VulkanGraphicsDevice::UnbindResources( uint32 slot, uint32 num, CommandList cmd ) { }

	void VulkanGraphicsDevice::UnbindUAVs( uint32 slot, uint32 num, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindSampler( EShaderStage stage, const Sampler* sampler, uint32 slot, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindConstantBuffer( EShaderStage stage, const Buffer* buffer, uint32 slot, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindVertexBuffers( const Buffer* const* vertexBuffers, uint32 slot, uint32 count, const uint32* strides, const uint32* offsets, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindIndexBuffer( const Buffer* indexBuffer, uint32 offset, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindStencilRef( uint32 value, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindBlendFactor( float r, float g, float b, float a, CommandList cmd ) { }

	void VulkanGraphicsDevice::BindComputeShader( const Shader* cs, CommandList cmd ) { }

	void VulkanGraphicsDevice::Draw( uint32 vertexCount, uint32 startVertexLocation, CommandList cmd ) { }

	void VulkanGraphicsDevice::DrawIndexed( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation, CommandList cmd ) { }

	void VulkanGraphicsDevice::DrawInstanced( uint32 vertexCount, uint32 instanceCount, uint32 startVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }

	void VulkanGraphicsDevice::DrawIndexedInstanced( uint32 indexCount, uint32 instanceCount, uint32 startIndexLocation, uint32 baseVertexLocation, uint32 startInstanceLocation, CommandList cmd ) { }

	void VulkanGraphicsDevice::DrawInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }

	void VulkanGraphicsDevice::DrawIndexedInstancedIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }
	
	void VulkanGraphicsDevice::Dispatch( uint32 threadGroupCountX, uint32 threadGroupCountY, uint32 threadGroupCountZ, CommandList cmd ) { }

	void VulkanGraphicsDevice::DispatchIndirect( const Buffer* args, uint32 args_offset, CommandList cmd ) { }

	void VulkanGraphicsDevice::CopyResource( const GraphicsDeviceResource* pDst, const GraphicsDeviceResource* pSrc, CommandList cmd ) { }

	void VulkanGraphicsDevice::UpdateBuffer( const Buffer* buffer, const void* data, CommandList cmd, int32 dataSize ) { }

    void VulkanGraphicsDevice::UpdateTexture( const Texture& texture, const void* data )
    {
        auto textureInternal = ToInternal( texture );

        // SDL_UpdateTexture(
        //     textureInternal->resource,
        //     NULL, data, (int)(texture.description.width * sizeof( uint32 ))
        // );
    }

	GraphicsDevice::DeviceAllocation VulkanGraphicsDevice::AllocateToDevice( size_t dataSize, CommandList cmd ) { return DeviceAllocation(); }

	void VulkanGraphicsDevice::EventBegin( const char* name, CommandList cmd ) { }

	void VulkanGraphicsDevice::EventEnd( CommandList cmd ) { }

	void VulkanGraphicsDevice::SetMarker( const char* name, CommandList cmd ) { }
}