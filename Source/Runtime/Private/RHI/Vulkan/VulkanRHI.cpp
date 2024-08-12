
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"

#include "RHI/Vulkan/VulkanRHI.h"

// #define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>
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
    static VkShaderStageFlagBits ConvertShaderStage( EShaderStage shaderStage )
    {
        switch ( shaderStage )
        {
        case ShaderStage_Tesselation:       return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage_TesselationEval:   return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case ShaderStage_Vertex:            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage_Geometry:          return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage_Fragment:          return VK_SHADER_STAGE_FRAGMENT_BIT;
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
            if ( colorspace == ColorSpace_Linear ) { return VK_FORMAT_R8G8B8A8_UNORM; }
            return VK_FORMAT_R8G8B8A8_SRGB;
        case PixelFormat_DepthComponent24:                 return VK_FORMAT_D24_UNORM_S8_UINT;
        case PixelFormat_DepthStencil:                     return VK_FORMAT_D16_UNORM_S8_UINT;
        case PixelFormat_ShadowDepth:                      return VK_FORMAT_D16_UNORM;
        default:
            return VK_FORMAT_UNDEFINED;
        }
    }

    static EPixelFormat ConvertImageFormat( VkFormat vkFormat )
    {
        switch ( vkFormat )
        {
        case VK_FORMAT_R8G8B8A8_UNORM:
        case VK_FORMAT_R8G8B8A8_SRGB:               return PixelFormat_R8G8B8A8_UNORM;
        // case VK_FORMAT_B8G8R8A8_UNORM:
        // case VK_FORMAT_B8G8R8A8_SRGB:               return PixelFormat_B8G8R8A8_UNORM;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:    return PixelFormat_A2R10G10B10_UNORM;
        // case VK_FORMAT_A2B10G10R10_UNORM_PACK32:    return PixelFormat_A2B10G10R10_UNORM;
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

    VkPresentModeKHR ConvertPresentMode( EPresentMode type )
    {
        switch ( type )
        {
        case PresentMode_Inmediate:     return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case PresentMode_VSync:         return VK_PRESENT_MODE_FIFO_KHR;
        default:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
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

    VkImageUsageFlags ConvertTextureUsage( EUsageModeFlags usage )
    {
        VkImageUsageFlags flags = VkImageUsageFlags( 0 );
        if ( usage & UsageMode_Sampled_Bit )    flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if ( usage & UsageMode_Storage_Bit )    flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        if ( usage & UsageMode_Color_Bit )      flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if ( usage & UsageMode_DepthStencil )   flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        return flags;
    }

    VkImageAspectFlags ConvertTextureAspect( EUsageModeFlags usage )
    {
        VkImageAspectFlags flags = VK_IMAGE_ASPECT_NONE;
        if ( usage & UsageMode_Color_Bit )      flags |= VK_IMAGE_ASPECT_COLOR_BIT;
        if ( usage & UsageMode_Depth_Bit )      flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
        if ( usage & UsageMode_Stencil_Bit )    flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
        return flags;
    }

    VkImageLayout ConvertTextureLayout( ETextureLayout layout )
    {
        switch ( layout )
        {
        case TextureLayout_Undefined:           return VK_IMAGE_LAYOUT_UNDEFINED;
        case TextureLayout_General:             return VK_IMAGE_LAYOUT_GENERAL;
        case TextureLayout_TransferSRC:         return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case TextureLayout_TransferDST:         return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case TextureLayout_ColorAttachment:     return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case TextureLayout_DepthAttachment:     return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        case TextureLayout_StencilAttachment:   return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case TextureLayout_Present:             return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
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
        VkBufferUsageFlags flags = 0;
        if ( usages & BufferUsage_SourceCopy_Bit )       flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if ( usages & BufferUsage_StreamOutput_Bit )     flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if ( usages & BufferUsage_Index_Bit )            flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if ( usages & BufferUsage_Vertex_Bit )           flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if ( usages & BufferUsage_Uniform_Bit )          flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if ( usages & BufferUsage_Storage_Bit )          flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if ( usages & BufferUsage_Indirect_Bit )         flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        return flags;
    }

    VkColorComponentFlags ConvertColorComponentFlags( EColorComponentFlags components )
    {
        VkColorComponentFlags flags = 0;
        if ( components & ColorComponent_R_Bit )     flags |= VK_COLOR_COMPONENT_R_BIT;
        if ( components & ColorComponent_G_Bit )     flags |= VK_COLOR_COMPONENT_G_BIT;
        if ( components & ColorComponent_B_Bit )     flags |= VK_COLOR_COMPONENT_B_BIT;
        if ( components & ColorComponent_A_Bit )     flags |= VK_COLOR_COMPONENT_A_BIT;
        return flags;
    }

    VkPipelineStageFlags ConvertPipelineStageFlags( EPipelineStage stage )
    {
        switch ( stage )
        {
        case PipelineStage_OutputColor: return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case PipelineStage_Top:         return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        case PipelineStage_None:
        default:
            return VK_PIPELINE_STAGE_NONE;
        }
    }

    static VkPrimitiveTopology ConvertDrawMode( ETopologyMode mode )
    {
        switch ( mode )
        {
        case TopologyMode_PointList:        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case TopologyMode_LineList:         return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case TopologyMode_LineStrip:        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case TopologyMode_TriangleList:     return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case TopologyMode_TriangleStrip:    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case TopologyMode_TriangleFan:      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        default:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        }
    }

    static VkPolygonMode ConvertRasterMode( ERasterMode mode )
    {
        switch ( mode )
        {
        case RasterMode_Solid:        return VK_POLYGON_MODE_FILL;
        case RasterMode_Wireframe:    return VK_POLYGON_MODE_LINE;
        case RasterMode_Point:        return VK_POLYGON_MODE_POINT;
        default:
            return VK_POLYGON_MODE_FILL;
        }
    }

    static VkFrontFace ConvertFrontFace( EFaceWinding mode )
    {
        switch ( mode )
        {
        case RasterMode_Solid:        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case RasterMode_Wireframe:    return VK_FRONT_FACE_CLOCKWISE;
        default:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }
    }

    static VkCullModeFlags ConvertCullMode( ECullModeFlags mode )
    {
        switch ( mode )
        {
            case CullMode_None:         return VK_CULL_MODE_NONE;
            case CullMode_Back_Bit:     return VK_CULL_MODE_BACK_BIT;
            case CullMode_Front_Bit:    return VK_CULL_MODE_FRONT_BIT;
            case CullMode_All:
            default:
                return VK_CULL_MODE_FRONT_AND_BACK;
        }
    }

    static VkBlendFactor ConvertBlendFactor( EBlendFactor factor )
    {
        switch ( factor )
        {
        case BlendFactor_Zero:              return VK_BLEND_FACTOR_ZERO;
        case BlendFactor_One:               return VK_BLEND_FACTOR_ONE;
        case BlendFactor_SrcColor:          return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor_SrcAlpha:          return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor_DstAlpha:          return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor_DstColor:          return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor_OneMinusSrcColor:  return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor_OneMinusSrcAlpha:  return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor_OneMinusDstAlpha:  return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor_OneMinusDstColor:  return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        default:
            return VK_BLEND_FACTOR_MAX_ENUM;
        }
    }

    static VkBlendOp ConvertBlendOperation( EBlendOperation operation )
    {
        switch ( operation )
        {
        case BlendOperation_Add:                return VK_BLEND_OP_ADD;
        case BlendOperation_Substract:          return VK_BLEND_OP_SUBTRACT;
        case BlendOperation_ReverseSubstract:   return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOperation_Min:                return VK_BLEND_OP_MIN;
        case BlendOperation_Max:                return VK_BLEND_OP_MAX;
        default:
            return VK_BLEND_OP_MAX_ENUM;
        }
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

    struct QueueFamilyIndices
    {
        std::optional<uint32> graphicsFamily;
        std::optional<uint32> presentFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
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
}

#include "RHI/Vulkan/VulkanRHIObjects.h"

namespace EE
{
    VulkanRHIDevice* GVulkanDevice;
    VulkanRHIInstance* GVulkanInstance;
#ifdef EMPTYENGINE_CORE_LOG
    VkDebugUtilsMessengerEXT GVulkanDebugMessager = VK_NULL_HANDLE;

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT*      callbackData,
        void* pUserData)
    {
        if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT )
        {
            EE_LOG_CORE_ERROR( L"Vulkan: {}", Text::NarrowToWide( callbackData->pMessage ) );
            return VK_FALSE;
        }
        if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
        {
            EE_LOG_CORE_WARN( L"Vulkan: {}", Text::NarrowToWide( callbackData->pMessage ) );
            return VK_FALSE;
        }
        if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT )
        {
            EE_LOG_CORE_INFO( L"Vulkan: {}", Text::NarrowToWide( callbackData->pMessage ) );
            return VK_FALSE;
        }
        if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT )
        {
            EE_LOG_CORE_INFO( L"Vulkan: {}", Text::NarrowToWide( callbackData->pMessage ) );
            return VK_FALSE;
        }
        return VK_FALSE;
    }
#endif

    const TArray<const NChar*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const float QueuePriorities[] = { 1.0F };

    VulkanRHIPhysicalDevice::VulkanRHIPhysicalDevice( VkInstance instance, VkPhysicalDevice physicalDevice ) :
        instance( instance ),
        physicalDevice( physicalDevice )
    {
        vkGetPhysicalDeviceProperties( physicalDevice, &deviceProperties );
        vkGetPhysicalDeviceFeatures( physicalDevice, &deviceFeatures );

        // Logic to find queue family indices to populate struct with
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, VK_NULL_HANDLE );
        queueFamilies.resize( queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice, &queueFamilyCount, queueFamilies.data() );

        // Get extension support
        vkEnumerateDeviceExtensionProperties( physicalDevice, VK_NULL_HANDLE, &extensionCount, VK_NULL_HANDLE );
        extensions.resize( extensionCount );
        vkEnumerateDeviceExtensionProperties( physicalDevice, VK_NULL_HANDLE, &extensionCount, extensions.data() );
    }

    VulkanRHIPhysicalDevice::~VulkanRHIPhysicalDevice()
    {

    }

    bool VulkanRHIPhysicalDevice::CheckDeviceExtensionSupport() const
    {
        std::set<std::string> requiredExtensions( DeviceExtensions.begin(), DeviceExtensions.end() );

        for ( const auto& extension : extensions )
        {
            requiredExtensions.erase( extension.extensionName );
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices VulkanRHIPhysicalDevice::GetQueueFamilies() const
    {
        QueueFamilyIndices indices;

        int i = 0;
        for ( const auto& queueFamily : queueFamilies )
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

    bool VulkanRHIPhysicalDevice::IsDeviceSuitable( VulkanRHISurface* surface ) const
    {
        QueueFamilyIndices indices = GetQueueFamilies();
        bool extensionsSupported = CheckDeviceExtensionSupport();

        bool swapChainAdequate = false;
        if ( auto search = surfaceSupportDetails.find( surface->GetVulkanSurface() ); search != surfaceSupportDetails.end() )
        {
            if ( search->second.supported )
            {
                swapChainAdequate = HasSurfaceAnyFormat( surface->GetVulkanSurface() ) && HasSurfaceAnyPresentMode( surface->GetVulkanSurface() );
            }
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    int VulkanRHIPhysicalDevice::RateDeviceSuitabilityForSurface( VulkanRHISurface* surface ) const
    {
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

        if ( IsDeviceSuitable( surface ) == false )
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

    void VulkanRHIPhysicalDevice::AddSurfaceSupportDetails( VkSurfaceKHR surface, uint32 queueFamilyIndex )
    {
        VulkanSurfaceSupportDetails surfaceDetails;

        vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice, queueFamilyIndex, surface, &surfaceDetails.supported );

        surfaceSupportDetails.insert( std::make_pair( surface, surfaceDetails ) );

        if ( surfaceDetails.supported == VK_FALSE )
        {
            return;
        }

        UpdateSurfaceSupportDetails( surface );
    }

    void VulkanRHIPhysicalDevice::UpdateSurfaceSupportDetails( VkSurfaceKHR surface )
    {
        if ( auto search = surfaceSupportDetails.find( surface ); search != surfaceSupportDetails.end() )
        {
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice, surface, &search->second.capabilities );

            vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, surface, &search->second.formatCount, VK_NULL_HANDLE );
            if ( search->second.formatCount != 0 )
            {
                search->second.formats.resize( search->second.formatCount );
                vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice, surface, &search->second.formatCount, search->second.formats.data() );
            }

            vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, surface, &search->second.presentModeCount, VK_NULL_HANDLE );
            if ( search->second.presentModeCount != 0 )
            {
                search->second.presentModes.resize( search->second.presentModeCount );
                vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice, surface, &search->second.presentModeCount, search->second.presentModes.data() );
            }
        }
    };

    const VulkanSurfaceSupportDetails* VulkanRHIPhysicalDevice::GetSurfaceSupportDetails( VkSurfaceKHR surface ) const
    {
        if ( auto search = surfaceSupportDetails.find( surface ); search != surfaceSupportDetails.end() )
            return NULL;
        else
            return &search->second;
    };

    VulkanRHIInstance::VulkanRHIInstance( VkInstanceCreateInfo& createInfo ) :
        RHIInstance::RHIInstance(),
        instance( VK_NULL_HANDLE ),
        physicalDeviceCount( 0 ),
        physicalDevices(),
        selectedDeviceIndex()
    {
        VkResult createResult;
        if ( (createResult = vkCreateInstance( &createInfo, VK_NULL_HANDLE, &instance )) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"vkCreateInstance failed : {}", (int32)createResult );
            return;
        }

        // Get physical devices
        vkEnumeratePhysicalDevices( instance, &physicalDeviceCount, VK_NULL_HANDLE );
        if ( physicalDeviceCount == 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find GPUs with Vulkan support!" );
            return;
        }

        EE_LOG_CORE_INFO( L"\u250C> Available devices : {}", physicalDeviceCount );

        // Get physical device info
        TArray<VkPhysicalDevice> vulkanPhysicalDevices( physicalDeviceCount );
        vkEnumeratePhysicalDevices( instance, &physicalDeviceCount, vulkanPhysicalDevices.data() );

        for ( uint32 i = 0; i < physicalDeviceCount; i++ )
        {
            VkPhysicalDevice& vulkanPhysicalDevice = vulkanPhysicalDevices[ i ];
            VulkanRHIPhysicalDevice& physicalDevice = physicalDevices.emplace_back( instance, vulkanPhysicalDevice );
        }
    }

    VulkanRHIInstance::~VulkanRHIInstance()
    {
        vkDestroyInstance( instance, VK_NULL_HANDLE );
    }

    int32 VulkanRHIInstance::PickPhysicalDeviceForSurface( VulkanRHISurface* surface ) const
    {
        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, int> candidates;

        for ( uint32 i = 0; i < physicalDeviceCount; i++ )
        {
            int score = physicalDevices[ i ].RateDeviceSuitabilityForSurface( surface );
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

    bool VulkanRHIInstance::SelectSuitableDevice( VulkanRHISurface* surface )
    {
        int selection = PickPhysicalDeviceForSurface( surface );

        if ( selection < 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find a suitable device!" );
            return false;
        }

        selectedDeviceIndex = selection;

        return true;
    }

    const VulkanRHIPhysicalDevice* VulkanRHIInstance::GetSelectedPhysicalDevice() const
    {
        return &physicalDevices[ selectedDeviceIndex ];
    };

    VulkanRHIPhysicalDevice* VulkanRHIInstance::GetSelectedPhysicalDevice()
    {
        return &physicalDevices[ selectedDeviceIndex ];
    };

    bool VulkanRHIInstance::AddSurfaceSupportDetails( VulkanRHISurface* surface )
    {
        for ( uint32 i = 0; i < physicalDeviceCount; i++ )
        {
            auto& internalPhysicalDevice = physicalDevices[ i ];
            internalPhysicalDevice.AddSurfaceSupportDetails( surface->GetVulkanSurface(), internalPhysicalDevice.GetQueueFamilies().graphicsFamily.value() );
        }
        return true;
    }

    bool VulkanRHIInstance::IsValid() const { return instance != VK_NULL_HANDLE; }
    
    const VulkanRHICommandPool* VulkanRHIDevice::GetCommandPool( uint32 familyIndex ) const { return commandPools.find( familyIndex )->second; }

    VulkanRHIDevice::~VulkanRHIDevice()
    {
        for ( auto& pair : commandPools )
            delete pair.second;

        delete presentQueue;
        delete graphicsQueue;
        
        vmaDestroyAllocator( allocator );
        vkDestroyDevice( device, nullptr );
    }

    VulkanRHIDevice::VulkanRHIDevice( VulkanRHIInstance* instance )
        : physicalDevice( instance->GetSelectedPhysicalDevice() ),
        presentQueue( NULL ), graphicsQueue( NULL )
    {
        EE_CORE_ASSERT( GVulkanDevice == NULL, L"Creating a second device!, Aborting..." );

        // Specifying the queues to be created
        QueueFamilyIndices indices = physicalDevice->GetQueueFamilies();

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = QueuePriorities[ 0 ];
        for ( uint32 queueFamily : uniqueQueueFamilies )
        {
            VkDeviceQueueCreateInfo queueCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = queueFamily,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority
            };
            queueCreateInfos.push_back( queueCreateInfo );
        }

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceVulkan13Features deviceFeatures13
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .dynamicRendering = VK_TRUE
        };

        VkPhysicalDeviceFeatures deviceFeatures
        {
            .geometryShader = VK_TRUE,
            .samplerAnisotropy = VK_TRUE
        };

        VkDeviceCreateInfo deviceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &deviceFeatures13,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0,         //   deprecated?
            .ppEnabledLayerNames = NULL,    //   deprecated?
            .enabledExtensionCount = (uint32)DeviceExtensions.size(),
            .ppEnabledExtensionNames = DeviceExtensions.data(),
            .pEnabledFeatures = &deviceFeatures
        };

        EE_CORE_ASSERT
        (
            vkCreateDevice( physicalDevice->GetPhysicalDevice(), &deviceCreateInfo, nullptr, &device ) == VK_SUCCESS,
            L"Failed to create logical device!"
        );

        EE_CORE_ASSERT
        (
            CreateNativeVmaAllocator( instance->GetVulkanInstance(), physicalDevice->GetPhysicalDevice(), device, &allocator ) == VK_SUCCESS,
            L"Failed to create VMA Allocator!"
        );

        graphicsQueue = new VulkanRHIQueue( this, graphicsQueueIndex = indices.graphicsFamily.value(), 0 );
        presentQueue = new VulkanRHIQueue( this, presentQueueIndex = indices.presentFamily.value(), 0 );

        if ( CreateCommandBufferPools( this ) == false )
        {
            return;
        }

        GVulkanDevice = this;
    }

    bool VulkanRHIDevice::IsValid() const { return device != VK_NULL_HANDLE; }

    bool VulkanRHIDevice::CreateCommandBufferPools( VulkanRHIDevice* device )
    {
        std::set<uint32> uniqueQueueFamilies = { device->graphicsQueueIndex, device->presentQueueIndex };

        for ( uint32 queueFamily : uniqueQueueFamilies )
        {
            device->commandPools.emplace( queueFamily, new VulkanRHICommandPool( device, queueFamily ) );
        }

        return true;
    }

    VulkanRHIPresentContext::~VulkanRHIPresentContext()
    {
        // Wait for all commands of this context
        for ( auto& fence : renderFences )
        {
            fence.Wait( UINT64_MAX );
        }

        delete swapChain;
        commandBuffers.clear();
        presentSemaphores.clear();
        renderSemaphores.clear();
        renderFences.clear();
        delete surface;
    }

    VulkanRHIPresentContext::VulkanRHIPresentContext( Window* window, VulkanRHIInstance* instance ) : RHIPresentContext(),
        window( window ), instance( instance ),
        swapChain( NULL ), surface( NULL ), commandBuffers()
    {
        CreateSurface();
        CreateSwapChain();
        CreateCommandBuffers();
        CreateSyncObjects();
    }

    const VulkanRHISemaphore& VulkanRHIPresentContext::GetPresentSempahore( uint32 frameIndex ) const
    {
        TList<VulkanRHISemaphore>::const_iterator semaphoresIt = presentSemaphores.begin();
        for ( uint32 i = 0; i < frameIndex; i++ ) ++semaphoresIt;
        return *semaphoresIt;
    }

    const VulkanRHISemaphore& VulkanRHIPresentContext::GetRenderSempahore( uint32 frameIndex ) const
    {
        TList<VulkanRHISemaphore>::const_iterator semaphoresIt = renderSemaphores.begin();
        for ( uint32 i = 0; i < frameIndex; i++ ) ++semaphoresIt;
        return *semaphoresIt;
    }

    void VulkanRHIPresentContext::AquireBackbuffer( uint64 timeout )
    {
        GetRenderFence()->Wait( timeout );
        bool success = swapChain->AquireNextImage( timeout, &GetPresentSempahore( CurrentFrameIndex() ), NULL );
        if ( success == false )
        {
            RecreateSwapChain();
        }
        GetRenderFence()->Reset();
    }

    void VulkanRHIPresentContext::Present()
    {
        bool success = SubmitPresentImage( GVulkanDevice->GetVulkanPresentQueue() );
        if ( success == false )
        {
            RecreateSwapChain();
        }
        currentFrameIndex = ( currentFrameIndex + 1 ) % swapChain->GetImageCount();
    }

    bool VulkanRHIPresentContext::SubmitPresentImage( VulkanRHIQueue* queue ) const
    {
        auto& renderSemaphore = GetRenderSempahore( CurrentFrameIndex() );
        VkPresentInfoKHR presentInfo
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = NULL,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &renderSemaphore.GetVulkanSemaphore(),
            .swapchainCount = 1,
            .pSwapchains = &swapChain->GetVulkanSwapChain(),
            .pImageIndices = &swapChain->BackImageIndex(),
            .pResults = NULL
        };

        VkResult result = vkQueuePresentKHR( queue->GetVulkanQueue(), &presentInfo );
        if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR )
        {
            return false;
        }
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to present image: {}! {}", swapChain->BackImageIndex(), (int32)result );
        }
        return true;
    }

    void VulkanRHIPresentContext::SubmitRenderCommandBuffer( EPipelineStage stage ) const
    {
        TList<VulkanRHICommandBuffer>::const_iterator commandBuffersIt = commandBuffers.begin();
        TList<VulkanRHISemaphore>::const_iterator presentSemaphoresIt = presentSemaphores.begin();
        TList<VulkanRHISemaphore>::const_iterator renderSemaphoresIt = renderSemaphores.begin();
        TList<VulkanRHIFence>::const_iterator renderFencesIt = renderFences.begin();
        const uint32& imageIndex = CurrentFrameIndex();
        for ( uint32 i = 0; i < imageIndex; i++ )
        {
            commandBuffersIt++;
            presentSemaphoresIt++;
            renderSemaphoresIt++;
            renderFencesIt++;
        }

        RHIQueueSubmitInfo ququeSubmitInfo{ };
        ququeSubmitInfo.waitSemaphores.emplace_back( &*presentSemaphoresIt );
        ququeSubmitInfo.signalSemaphores.emplace_back( &*renderSemaphoresIt );
        ququeSubmitInfo.signalFence = &*renderFencesIt;
        ququeSubmitInfo.stageFlags = stage;
        GVulkanDevice->GetVulkanPresentQueue()->SubmitCommandBuffer( &*commandBuffersIt, ququeSubmitInfo );
    }

    const RHICommandBuffer* VulkanRHIPresentContext::GetRenderCommandBuffer() const
    {
        const uint32& imageIndex = CurrentFrameIndex();
        TList<VulkanRHICommandBuffer>::const_iterator commandBuffersIt = commandBuffers.begin();
        for ( uint32 i = 0; i < imageIndex; i++ ) ++commandBuffersIt;
        return &*commandBuffersIt;
    }

    const RHIFence* VulkanRHIPresentContext::GetRenderFence() const
    {
        const uint32& imageIndex = CurrentFrameIndex();
        TList<VulkanRHIFence>::const_iterator renderFencesIt = renderFences.begin();
        for ( uint32 i = 0; i < imageIndex; i++ ) ++renderFencesIt;
        return &*renderFencesIt;
    }

    const RHITexture* VulkanRHIPresentContext::GetBackbuffer() const
    {
        return swapChain->GetTexture( swapChain->BackImageIndex() );
    }

    const EPixelFormat& VulkanRHIPresentContext::GetFormat() const
    {
        return swapChain->GetTexture( 0 )->GetFormat();
    }

    void VulkanRHIPresentContext::CreateSurface()
    {
        surface = new VulkanRHISurface( window, instance );

        instance->AddSurfaceSupportDetails( surface );
        // Vulkan needs the surface information in order to determine the driver, so the driver creation is here, the device selection is handled inside CreateSurface
        if ( GVulkanDevice == NULL )
        {
            if ( instance->SelectSuitableDevice( surface ) == false )
            {
                return;
            }

            GVulkanDevice = new VulkanRHIDevice( instance );
        }
    }

    void VulkanRHIPresentContext::CreateSwapChain()
    {
        SDL_PropertiesID displayProperties = SDL_GetDisplayProperties( SDL_GetPrimaryDisplay() );

        bool tryHDR = false;
        if ( window->GetAllowHDR() && SDL_GetBooleanProperty( displayProperties, SDL_PROP_DISPLAY_HDR_ENABLED_BOOLEAN, SDL_FALSE ) )
        {
            tryHDR = true;
        }

        EPixelFormat desiredFormat = PixelFormat_R8G8B8A8_UNORM;
        EColorSpace desiredColorSpace = ColorSpace_sRGB;
        GetSurfaceColorFormat( tryHDR, &desiredFormat, &desiredColorSpace );

        RHISwapChainCreateInfo swapChainInfo
        {
            .window = this->window,
            .width = (uint32)window->GetWidth(),
            .height = (uint32)window->GetHeight(),
            .bufferCount = 2,
            .format = desiredFormat,
            .colorSpace = desiredColorSpace,
            .presentMode = window->GetPresentMode(),
        };

        swapChain = new VulkanRHISwapChain( swapChainInfo, this, GVulkanDevice );
    }

    void VulkanRHIPresentContext::RecreateSwapChain()
    {
        // Wait for all commands of this context
        for ( auto& fence : renderFences )
        {
            fence.Wait( UINT64_MAX );
        }

        swapChain->Cleanup();
        GVulkanDevice->GetVulkanPhysicalDevice()->UpdateSurfaceSupportDetails( surface->GetVulkanSurface() );

        SDL_PropertiesID displayProperties = SDL_GetDisplayProperties( SDL_GetPrimaryDisplay() );

        bool tryHDR = false;
        if ( window->GetAllowHDR() && SDL_GetBooleanProperty( displayProperties, SDL_PROP_DISPLAY_HDR_ENABLED_BOOLEAN, SDL_FALSE ) )
        {
            tryHDR = true;
        }

        EPixelFormat desiredFormat = PixelFormat_R8G8B8A8_UNORM;
        EColorSpace desiredColorSpace = ColorSpace_sRGB;
        GetSurfaceColorFormat( tryHDR, &desiredFormat, &desiredColorSpace );

        RHISwapChainCreateInfo swapChainInfo
        {
            .window = window,
            .width = (uint32)window->GetWidth(),
            .height = (uint32)window->GetHeight(),
            .bufferCount = 2,
            .format = desiredFormat,
            .colorSpace = desiredColorSpace,
            .presentMode = window->GetPresentMode(),
        };

        swapChain->CreateSwapChain( swapChainInfo );
        // currentFrameIndex = swapChain->GetImageCount() - 1;
    }

    void VulkanRHIPresentContext::GetSurfaceColorFormat( bool hdr, EPixelFormat* outFormat, EColorSpace* outColorSpace ) const
    {
        auto& surfaceDetails = GVulkanDevice->GetVulkanPhysicalDevice()->GetSurfaceDetails( surface->GetVulkanSurface() );
        // Find best format
        bool contains = false;
        for ( uint32 i = 0; i < surfaceDetails.formatCount; i++ )
        {
            const VkSurfaceFormatKHR& surfaceFormat = surfaceDetails.formats[ i ];
            bool colorSpaceHDR = FrameColorSpaceIsHDR( surfaceFormat.colorSpace );
            EColorSpace colorSpace = ConvertColorSpace( surfaceFormat.colorSpace );
            EPixelFormat format = ConvertImageFormat( surfaceFormat.format );

            if ( colorSpace == ColorSpace_Unknown )
                continue;
            if ( format == PixelFormat_Unknown )
                continue;

            if ( hdr )
            {
                if ( colorSpaceHDR )
                {
                    *outFormat = format;
                    *outColorSpace = colorSpace;
                    break;
                }

                if ( colorSpace == ColorSpace_sRGB )
                {
                    if ( format == PixelFormat_A2R10G10B10_UNORM || format == PixelFormat_R16G16B16A16_SNORM )
                    {
                        *outFormat = format;
                        *outColorSpace = colorSpace;
                        break;
                    }
                }

                if ( colorSpaceHDR == false )
                {
                    *outFormat = format;
                    *outColorSpace = colorSpace;
                }
            }
            else
            {
                if ( colorSpaceHDR == false )
                {
                    *outFormat = format;
                    *outColorSpace = colorSpace;
                    break;
                }
            }
        }
    }

    void VulkanRHIPresentContext::CreateCommandBuffers()
    {
        RHICommandBufferCreateInfo createInfo{};

        const uint32& imageCount = swapChain->GetImageCount();
        for ( uint32 i = 0; i < imageCount; i++ )
        {
            commandBuffers.emplace_back( createInfo, GVulkanDevice, GVulkanDevice->GetPresentFamilyIndex() );
        }
    }

    void VulkanRHIPresentContext::CreateSyncObjects()
    {
        const uint32& imageCount = swapChain->GetImageCount();
        for ( uint32 i = 0; i < imageCount; i++ )
        {
            presentSemaphores.emplace_back( GVulkanDevice );
            renderSemaphores.emplace_back( GVulkanDevice );
            renderFences.emplace_back( GVulkanDevice, true );
        }
    }

    bool VulkanRHIPresentContext::IsValid() const
    {
        return surface->IsValid() && swapChain->IsValid();
    }

    VulkanRHIQueue::~VulkanRHIQueue()
    {
    }

    VulkanRHIQueue::VulkanRHIQueue( const VulkanRHIDevice* device, const uint32& familyIndex, const uint32& queueIndex ) :
        device( device ) 
    {
        vkGetDeviceQueue( device->GetVulkanDevice(), familyIndex, queueIndex, &queue);
    }

    bool VulkanRHIQueue::IsValid() const
    {
        return queue != VK_NULL_HANDLE;
    }

    void VulkanRHIQueue::WaitIdle() const
    {
        vkQueueWaitIdle( queue );
    }

    void VulkanRHIQueue::SubmitCommandBuffer( const RHICommandBuffer* commandBuffer, const RHIQueueSubmitInfo& info )
    {
        auto vulkanCommandBuffer = static_cast<const VulkanRHICommandBuffer*>( commandBuffer );
        auto vulkanFence = static_cast<const VulkanRHIFence*>( info.signalFence );

        uint32 waitSemaphoreCount = (uint32)info.waitSemaphores.size();
        std::vector<VkSemaphore> waitSemaphores;
        std::vector<VkPipelineStageFlags> waitStageFlags;
        waitSemaphores.resize( waitSemaphoreCount );
        waitStageFlags.resize( waitSemaphoreCount );
        for ( uint32 i = 0; i < waitSemaphoreCount; i++ )
        {
            auto* vkSemaphore = static_cast<const VulkanRHISemaphore*>(info.waitSemaphores[ i ]);
            waitSemaphores[ i ] = vkSemaphore->GetVulkanSemaphore();
            waitStageFlags[ i ] = ConvertPipelineStageFlags( info.stageFlags );
        }

        uint32 signalSemaphoreCount = (uint32)info.signalSemaphores.size();
        std::vector<VkSemaphore> signalSemaphores;
        signalSemaphores.resize( signalSemaphoreCount );
        for ( uint32 i = 0; i < signalSemaphoreCount; i++ )
        {
            auto* vkSemaphore = static_cast<const VulkanRHISemaphore*>(info.signalSemaphores[ i ]);
            signalSemaphores[ i ] = vkSemaphore->GetVulkanSemaphore();
        }

        VkSubmitInfo vkSubmitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = NULL,
            .waitSemaphoreCount = waitSemaphoreCount,
            .pWaitSemaphores = waitSemaphores.data(),
            .pWaitDstStageMask = waitStageFlags.data(),
            .commandBufferCount = 1,
            .pCommandBuffers = &vulkanCommandBuffer->GetVulkanCommandBuffer(),
            .signalSemaphoreCount = signalSemaphoreCount,
            .pSignalSemaphores = signalSemaphores.data()
        };

        VkFence nativeFence = vulkanFence == NULL ? VK_NULL_HANDLE : vulkanFence->GetVulkanFence();
        auto result = vkQueueSubmit( queue, 1, &vkSubmitInfo, nativeFence );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed command vkQueueSubmit! {}", (uint32)result );
        }
    }

    VulkanRHIBuffer::~VulkanRHIBuffer()
    {
        if ( buffer != NULL )
        {
            vkDestroyBuffer( device->GetVulkanDevice(), buffer, NULL );
        }
    }

    VulkanRHIBuffer::VulkanRHIBuffer( RHIBufferCreateInfo& info, VulkanRHIDevice* device ) :
        device( device ), usages( BufferUsage_None )
    {
        VkBufferCreateInfo bufferInfo = 
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .size = info.size,
            .usage = ConvertBufferUsageFlags( info.usages ),
            .sharingMode = ConvertSharingMode( info.sharing ),
        };

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

        if ( vmaCreateBuffer( device->GetVulkanAllocator(), &bufferInfo, &allocInfo, &buffer, &nativeAllocation, VK_NULL_HANDLE ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to allocate buffer memory!" );
        }

    }

    VulkanRHITexture::VulkanRHITexture( const RHITextureCreateInfo& info, VulkanRHIDevice* device, VkImage image ) :
        device( device ),
        extent( info.width, info.height, info.depth ),
        format( ConvertPixelFormat( info.format, info.colorSpace ) ),
        sampler( VK_NULL_HANDLE ), memory( VK_NULL_HANDLE ),
        image( image ),
        ownership( false )
    {
        VkImageViewCreateInfo viewInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL, 
            .flags = 0,
            .image = image,
            .viewType = ConvertTextureType( info.type ),
            .format = format,
            .components = VkComponentMapping
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = VkImageSubresourceRange
            {
                .aspectMask = ConvertTextureAspect( info.viewUsage ),
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            } 
        };

        if ( vkCreateImageView( device->GetVulkanDevice(), &viewInfo, nullptr, &imageView ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create texture image view!" );
            return;
        }
    }

    VulkanRHITexture::VulkanRHITexture( const RHITextureCreateInfo& info, VulkanRHIDevice* device ) :
        device( device ),
        extent( info.width, info.height, info.depth ),
        format( ConvertPixelFormat( info.format, info.colorSpace ) ),
        ownership( true ),
        sampler( VK_NULL_HANDLE ), memory( VK_NULL_HANDLE ), imageView( VK_NULL_HANDLE )
    {
        VkImageCreateInfo imageInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = VkExtent3D {
                .width = extent.x,
                .height = extent.y,
                .depth = extent.z
            },
            . mipLevels = info.mipLevels,
            . arrayLayers = info.arraySize,
            . samples = (VkSampleCountFlagBits)info.sampleCount,
            . tiling = ConvertTextureTiling( info.tiling ),
            . usage = ConvertTextureUsage( info.usage ),
            . sharingMode = ConvertSharingMode( info.sharing ),
            . queueFamilyIndexCount = 1,
            . pQueueFamilyIndices = device->GetFamilyIndices(),
            . initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        if ( vkCreateImage( device->GetVulkanDevice(), &imageInfo, VK_NULL_HANDLE, &image ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create image!" );
            return;
        }

        VmaAllocationCreateInfo allocInfo
        {
            .flags = 0,
            .usage = VMA_MEMORY_USAGE_AUTO,
            .requiredFlags = 0,
            .preferredFlags = 0,
            .memoryTypeBits = 0,
            .pool = VK_NULL_HANDLE,
            .pUserData = NULL,
            .priority = 0.0F
        };

        if ( vmaCreateImage( device->GetVulkanAllocator(), &imageInfo, &allocInfo, &image, &memory, NULL ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to allocate image memory!" );
            return;
        }

        VkImageViewCreateInfo viewInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .image = image,
            .viewType = ConvertTextureType( info.type ),
            .format = format,
            .components = VkComponentMapping
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = VkImageSubresourceRange
            {
                .aspectMask = ConvertTextureAspect( info.viewUsage ),
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        if ( vkCreateImageView( device->GetVulkanDevice(), &viewInfo, nullptr, &imageView ) != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create texture image view!" );
            return;
        }
    }

    VulkanRHITexture::~VulkanRHITexture()
    {
        if ( image != VK_NULL_HANDLE && ownership )
        {
            vkDestroyImage( device->GetVulkanDevice(), image, NULL );
        }

        CleanImageView();
    }

    void VulkanRHITexture::CleanImageView() const
    {
        if ( imageView != VK_NULL_HANDLE )
        {
            vkDestroyImageView( device->GetVulkanDevice(), imageView, NULL );
        }
    }

    bool VulkanRHITexture::IsValid() const
    {
        return image != VK_NULL_HANDLE;
    }

    bool VulkanRHISwapChain::IsValid() const
    {
        return swapChain != VK_NULL_HANDLE;
    }

    VulkanRHISwapChain::VulkanRHISwapChain( const RHISwapChainCreateInfo& info, const VulkanRHIPresentContext* presentContext, VulkanRHIDevice* device ) :
        device( device ),
        presentContext( presentContext ),
        swapChain( VK_NULL_HANDLE ),
        imageCount(), size(),
        backImageIndex( -1 )
    {
        CreateSwapChain( info );
    }

    VulkanRHISwapChain::~VulkanRHISwapChain()
    {
        Cleanup();
    }

    void VulkanRHISwapChain::CreateSwapChain( const RHISwapChainCreateInfo& info )
    {
        const VulkanRHISurface* surface = presentContext->GetRHISurface();
        const VulkanSurfaceSupportDetails& surfaceDetails = device->GetVulkanPhysicalDevice()->GetSurfaceDetails( surface->GetVulkanSurface() );

        VkSurfaceFormatKHR desiredFormat;

        bool contains = false;
        for ( uint32 i = 0; i < surfaceDetails.formatCount; i++ )
        {
            const VkSurfaceFormatKHR& surfaceFormat = surfaceDetails.formats[ i ];
            if ( ConvertImageFormat( surfaceFormat.format ) == info.format )
            {
                if ( ConvertColorSpace( surfaceFormat.colorSpace ) == info.colorSpace )
                {
                    contains = true;
                    desiredFormat = surfaceFormat;
                }
            }
        }

        if ( contains == false )
        {
            EE_LOG_CORE_CRITICAL( L"Surface format {}, {} is not supported!", (uint32)info.format, (uint32)info.colorSpace );
            return;
        }

        uint32 width = info.width;
        uint32 height = info.height;
        width = EE_CLAMP( width, surfaceDetails.capabilities.minImageExtent.width, surfaceDetails.capabilities.maxImageExtent.width );
        height = EE_CLAMP( height, surfaceDetails.capabilities.minImageExtent.height, surfaceDetails.capabilities.maxImageExtent.height );
        size.width = width;
        size.height = height;

        imageCount = Math::Max( surfaceDetails.capabilities.minImageCount + 1, info.bufferCount );
        imageCount = Math::Min( surfaceDetails.capabilities.maxImageCount, info.bufferCount );

        VkSwapchainCreateInfoKHR createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = NULL,
            .flags = 0,
            .surface = surface->GetVulkanSurface(),
            .minImageCount = imageCount,
            .imageFormat = desiredFormat.format,
            .imageColorSpace = desiredFormat.colorSpace,
            .imageExtent = size,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        };

        if ( device->GetGraphicsFamilyIndex() != device->GetPresentFamilyIndex() )
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = device->GetFamilyIndices();
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 1;
            createInfo.pQueueFamilyIndices = device->GetFamilyIndices();
        }

        createInfo.preTransform = surfaceDetails.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = ConvertPresentMode( info.presentMode );
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        auto result = vkCreateSwapchainKHR( device->GetVulkanDevice(), &createInfo, NULL, &swapChain );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed vkCreateSwapchainKHR! {}", (int32)result );
        }

        vkGetSwapchainImagesKHR( device->GetVulkanDevice(), swapChain, &imageCount, NULL );
        images.resize( imageCount );
        vkGetSwapchainImagesKHR( device->GetVulkanDevice(), swapChain, &imageCount, images.data() );

        for ( uint32 i = 0; i < imageCount; i++ )
        {
            RHITextureCreateInfo textureCreateInfo
            {
                .type = TextureType_Texture2D,
                .width = width,
                .height = height,
                .depth = 0,
                .arraySize = 1,
                .mipLevels = 1,
                .format = info.format,
                .colorSpace = info.colorSpace
            };

            textures.emplace_back( new VulkanRHITexture( textureCreateInfo, device, images[ i ] ) );
        }
    }

    void VulkanRHISwapChain::Cleanup()
    {
        for ( VulkanRHITexture* texture : textures )
        {
            delete texture;
        }
        textures.clear();

        vkDestroySwapchainKHR( device->GetVulkanDevice(), swapChain, NULL );
    }

    bool VulkanRHISwapChain::AquireNextImage( uint64 timeout, const VulkanRHISemaphore* semaphore, const VulkanRHIFence* fence )
    {
        auto vulkanSemaphore = semaphore == NULL ? VK_NULL_HANDLE : semaphore->GetVulkanSemaphore();
        auto vulkanFence = fence == NULL ? VK_NULL_HANDLE : fence->GetVulkanFence();

        auto result = vkAcquireNextImageKHR( device->GetVulkanDevice(), swapChain, timeout, vulkanSemaphore, VK_NULL_HANDLE, &backImageIndex );
        if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR )
        {
            EE_LOG_CORE_INFO( L"Swap chain out of date!" );
            return false;
        }
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed vkAcquireNextImageKHR! {}", (int32)result );
        }

        return true;
    }

    VulkanRHISurface::~VulkanRHISurface()
    {
        vkDestroySurfaceKHR( instance->GetVulkanInstance(), surface, nullptr );
    }

    VulkanRHISurface::VulkanRHISurface( Window* window, VulkanRHIInstance* instance ) :
        instance( instance ),
        surface( VK_NULL_HANDLE )
    {
        if ( SDL_Vulkan_CreateSurface( (SDL_Window*)window->GetWindowHandle(), instance->GetVulkanInstance(), VK_NULL_HANDLE, &surface ) != 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed SDL_Vulkan_CreateSurface! {}", Text::NarrowToWide( SDL_GetError() ) );
            return;
        }
    }

    bool VulkanRHISurface::IsValid() const
    {
        return surface != VK_NULL_HANDLE;
    }

    VulkanRHIFence::~VulkanRHIFence()
    {
        vkDestroyFence( device->GetVulkanDevice(), fence, nullptr );
    }

    VulkanRHIFence::VulkanRHIFence( VulkanRHIDevice* device, bool initSignaled ) :
        device( device ),
        fence( VK_NULL_HANDLE )
    {
        VkFenceCreateInfo fenceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = NULL,
            .flags = initSignaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u
        };

        auto createResult = vkCreateFence( device->GetVulkanDevice(), &fenceCreateInfo, nullptr, &fence);
        if ( createResult != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to fence: {}", (int32)createResult );
        }
    }

    bool VulkanRHIFence::IsSignaled() const
    {
        return vkGetFenceStatus( device->GetVulkanDevice(), fence ) == VK_SUCCESS;
    }

    void VulkanRHIFence::Reset() const
    {
        auto result = vkResetFences( device->GetVulkanDevice(), 1, &fence );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed reset fence: {}", (int32)result );
        }
    }

    void VulkanRHIFence::Wait( uint64 timeout ) const
    {
        auto result = vkWaitForFences( device->GetVulkanDevice(), 1, &fence, VK_TRUE, timeout );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed wait fence: {}", (int32)result );
        }
    }

    bool VulkanRHIFence::IsValid() const
    {
        return fence != VK_NULL_HANDLE;
    }

    VulkanRHISemaphore::~VulkanRHISemaphore()
    {
        vkDestroySemaphore( device->GetVulkanDevice(), semaphore, nullptr );
    }
    
    VulkanRHISemaphore::VulkanRHISemaphore( VulkanRHIDevice* device ) :
        device( device ),
        semaphore( VK_NULL_HANDLE )
    {
        VkSemaphoreCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0
        };

        auto createResult = vkCreateSemaphore( device->GetVulkanDevice(), &createInfo, nullptr, &semaphore );
        if ( createResult != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create semaphore: {}", (int32)createResult );
        }
    }

    bool VulkanRHISemaphore::IsValid() const
    {
        return semaphore != VK_NULL_HANDLE;
    }

    VulkanRHICommandPool::~VulkanRHICommandPool()
    {
        if ( commandPool != VK_NULL_HANDLE )
        {
            vkDestroyCommandPool( device->GetVulkanDevice(), commandPool, nullptr );
        }
    }

    VulkanRHICommandPool::VulkanRHICommandPool( VulkanRHIDevice* device, uint32 queueFamilyIndex ) :
        device( device ),
        queueFamilyIndex( queueFamilyIndex )
    {
        VkCommandPoolCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queueFamilyIndex
        };

        auto createResult = vkCreateCommandPool( device->GetVulkanDevice(), &createInfo, NULL, &commandPool );
        if ( createResult != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create command pool: {}", (int32)createResult );
        }
    }

    VulkanRHICommandBuffer::~VulkanRHICommandBuffer()
    {
        vkFreeCommandBuffers( device->GetVulkanDevice(), device->GetCommandPool( queueFamilyIndex )->GetVulkanCommandPool(), 1, &commandBuffer );
    }

    VulkanRHICommandBuffer::VulkanRHICommandBuffer( const RHICommandBufferCreateInfo& info, VulkanRHIDevice* device, uint32 queueFamilyIndex ) :
        device( device ),
        queueFamilyIndex( queueFamilyIndex )
    {
        VkCommandBufferAllocateInfo allocateInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .commandPool = device->GetCommandPool( queueFamilyIndex )->GetVulkanCommandPool(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkResult result = vkAllocateCommandBuffers( device->GetVulkanDevice(), &allocateInfo, &commandBuffer );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Unable to allocate command buffer {}", (int32)result );
        }
    }

    bool VulkanRHICommandBuffer::IsValid() const
    {
        return commandBuffer != VK_NULL_HANDLE;
    }

    void VulkanRHICommandBuffer::Begin() const
    {
        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = NULL
        };

        auto result = vkBeginCommandBuffer( commandBuffer, &beginInfo );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to end command buffer {}", (int32)result );
        }
    }

    void VulkanRHICommandBuffer::End() const
    {
        auto result = vkEndCommandBuffer( commandBuffer );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to end command buffer {}", (int32)result );
        }
    }

    void VulkanRHICommandBuffer::TransitionTexture( const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer, const ETextureLayout from, const ETextureLayout to ) const
    {
        auto vulkanTexture = static_cast<const VulkanRHITexture*>(texture);

        VkImageLayout oldLayout = ConvertTextureLayout( from );
        VkImageLayout newLayout = ConvertTextureLayout( to );

        VkImageSubresourceRange imageRange
        {
            .aspectMask = (uint32)((newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT),
            .baseMipLevel = mipLevel,
            .levelCount = 1, // VK_REMAINING_MIP_LEVELS,
            .baseArrayLayer = arrayLayer,
            .layerCount = 1, // VK_REMAINING_ARRAY_LAYERS
        };

        VkImageMemoryBarrier imageBarrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = NULL,
            .srcAccessMask = (uint32)((oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL || oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) ? VK_ACCESS_TRANSFER_WRITE_BIT : VK_ACCESS_MEMORY_READ_BIT),
            .dstAccessMask = (uint32)((newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) ? VK_ACCESS_TRANSFER_WRITE_BIT : VK_ACCESS_MEMORY_READ_BIT),
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = vulkanTexture->GetVulkanImage(),
            .subresourceRange = imageRange
        };

        vkCmdPipelineBarrier( 
            commandBuffer,
            (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL || oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) ? VK_PIPELINE_STAGE_TRANSFER_BIT :
            (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) ? VK_PIPELINE_STAGE_TRANSFER_BIT :
            (newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            0,                  // dependency flags
            0, NULL,            // memory barriers
            0, NULL,            // buffer memory barriers
            1, &imageBarrier ); // image memory barriers
    }

    void VulkanRHICommandBuffer::ClearColor( Vector3f color, const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer ) const
    {
        VkClearColorValue clearColor = { color.r, color.g, color.b };

        VkImageSubresourceRange imageRange
        {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = mipLevel,
            .levelCount = 1,
            .baseArrayLayer = arrayLayer,
            .layerCount = 1
        };

        auto vulkanTexture = static_cast<const VulkanRHITexture*>( texture );

        vkCmdClearColorImage( commandBuffer, vulkanTexture->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &imageRange );
    }

    VulkanRHIShaderStage::~VulkanRHIShaderStage()
    {
        vkDestroyShaderModule( device->GetVulkanDevice(), shaderModule, nullptr );
    }

    VulkanRHIShaderStage::VulkanRHIShaderStage( const RHIShaderStageCreateInfo& info, VulkanRHIDevice* device ) : RHIShaderStage( info.stage )
        , device( device )
        , entryPoint( info.entryPoint )
    {
        VkShaderModuleCreateInfo createInfo
        {
             .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
             .pNext = NULL,
             .flags = 0,
             .codeSize = info.codeLength,
             .pCode = reinterpret_cast<const uint32*>( info.code )
        };

        if ( vkCreateShaderModule( device->GetVulkanDevice(), &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
        {
            EE_LOG_CORE_ERROR( "Failed to create shader module!" );
            return;
        }
    }

    bool VulkanRHIShaderStage::IsValid() const
    {
        return shaderModule != VK_NULL_HANDLE;
    }

    const NChar* VulkanRHIShaderStage::GetEntryPoint() const
    {
        return entryPoint;
    }

    VulkanRHIGraphicsPipeline::VulkanRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info, VulkanRHIDevice* device )
        : pipelineLayout( VK_NULL_HANDLE )
        , pipeline( VK_NULL_HANDLE )
        , device( device )
    {
        TArray<VkPipelineShaderStageCreateInfo> stages;
        auto AddStage = [&stages]( const RHIShaderStageAttachment& shaderAttachment )
            {
                if ( shaderAttachment.shaderStage == NULL ) return;

                const VulkanRHIShaderStage* vulkanShaderStage = static_cast<const VulkanRHIShaderStage*>(shaderAttachment.shaderStage);

                VkPipelineShaderStageCreateInfo shaderStageInfo
                {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = VK_NULL_HANDLE,
                    .flags = 0,
                    .stage = ConvertShaderStage( vulkanShaderStage->GetStage() ),
                    .module = vulkanShaderStage->GetVulkanShaderModule(),
                    .pName = vulkanShaderStage->GetEntryPoint(),
                    .pSpecializationInfo = NULL
                };
                stages.emplace_back( shaderStageInfo );
            };
        AddStage( info.vertexShader );
        AddStage( info.fragmentShader );
        AddStage( info.geometryShader );

        const VkDynamicState dynamicStates[] =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_CULL_MODE,
            VK_DYNAMIC_STATE_FRONT_FACE,
        };

        VkPipelineDynamicStateCreateInfo dynamicState
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .dynamicStateCount = EE_ARRAYSIZE( dynamicStates ),
            .pDynamicStates = (VkDynamicState*)&dynamicStates
        };
        
        VkPipelineVertexInputStateCreateInfo vertexInputInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = NULL, // Optional
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = NULL, // Optional
        };
        
        VkPipelineInputAssemblyStateCreateInfo inputAssembly
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .topology = ConvertDrawMode( info.primitiveState.topologyMode ),
            // Normally, the vertices are loaded from the vertex buffer by index in sequential order,
            // but with an element buffer you can specify the indices to use yourself.
            // This allows you to perform optimizations like reusing vertices. If you set the primitiveRestartEnable member to VK_TRUE,
            // then it's possible to break up lines and triangles in the _STRIP topology modes by using a special index of 0xFFFF or 0xFFFFFFFF.
            .primitiveRestartEnable = VK_FALSE
        }; 
        
        VkPipelineViewportStateCreateInfo viewportState
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = NULL, // Dynamic
            .scissorCount = 1,
            .pScissors = NULL   // Dynamic
        }; 
        
        VkPipelineRasterizationStateCreateInfo rasterizer
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            // If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them as opposed to discarding them.
            // This is useful in some special cases like shadow maps. Using this requires enabling a GPU feature.
            .depthClampEnable = VK_FALSE,
            // If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage.
            // This basically disables any output to the framebuffer.
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = ConvertRasterMode( info.primitiveState.rasterMode ),
            .cullMode = ConvertCullMode( info.primitiveState.cullMode ),
            .frontFace = ConvertFrontFace( info.primitiveState.frontFace ),
            // The rasterizer can alter the depth values by adding a constant value or biasing them based on a fragment's slope.
            // This is sometimes used for shadow mapping, but we won't be using it.
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0F,    // Optional
            .depthBiasClamp = 0.0F,             // Optional
            .depthBiasSlopeFactor = 0.0F,       // Optional
            // The lineWidth member is straightforward, it describes the thickness of lines in terms of number of fragments.
            // The maximum line width that is supported depends on the hardware and any line thicker than 1.0f requires you to enable the wideLines GPU feature.
            .lineWidth = 1.0F
        }; 

        // Enabling multisampling it requires enabling a GPU feature.
        VkPipelineMultisampleStateCreateInfo multisampling
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0F,           // Optional
            .pSampleMask = NULL,                // Optional
            .alphaToCoverageEnable = VK_FALSE,  // Optional
            .alphaToOneEnable = VK_FALSE,       // Optional
        }; 
        
        VkPipelineDepthStencilStateCreateInfo depStencil {};
        // {
        //     .sType
        //     .pNext
        //     .flags
        //     .depthTestEnable
        //     .depthWriteEnable
        //     .depthCompareOp
        //     .depthBoundsTestEnable
        //     .stencilTestEnable
        //     .front
        //     .back
        //     .minDepthBounds
        //     .maxDepthBounds
        // };

        uint32 colorAttachmentCount = (uint32)info.colorAttachments.size();
        TArray<VkPipelineColorBlendAttachmentState> colorAttachments( colorAttachmentCount );

        TArray<VkFormat> pixelFormats( colorAttachmentCount );
        for ( uint32 i = 0; i < colorAttachmentCount; i++ )
        {
            auto format = info.colorAttachments[ i ].format;
            pixelFormats[ i ] = ConvertPixelFormat( info.colorAttachments[ i ].format, EColorSpace::ColorSpace_sRGB /*info.colorAttachments[ i ].colorSpace */ );
        }

        for ( uint32 i = 0; i < colorAttachmentCount; ++i )
        {
            const RHIColorAttachmentState& state = info.colorAttachments[ i ];
            VkPipelineColorBlendAttachmentState& colorBlendAttachment = colorAttachments[ i ];
            colorBlendAttachment.blendEnable = state.blendEnabled ? VK_TRUE : VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = ConvertBlendFactor( state.colorBlend.srcFactor );
            colorBlendAttachment.dstColorBlendFactor = ConvertBlendFactor( state.colorBlend.dstFactor );
            colorBlendAttachment.colorBlendOp = ConvertBlendOperation( state.colorBlend.operation );
            colorBlendAttachment.srcAlphaBlendFactor = ConvertBlendFactor( state.alphaBlend.srcFactor );
            colorBlendAttachment.dstAlphaBlendFactor = ConvertBlendFactor( state.alphaBlend.dstFactor );
            colorBlendAttachment.alphaBlendOp = ConvertBlendOperation( state.alphaBlend.operation );
            colorBlendAttachment.colorWriteMask = ConvertColorComponentFlags( state.writeFlags );
        }

        VkPipelineColorBlendStateCreateInfo colorInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_CLEAR,
            .attachmentCount = colorAttachmentCount,
            .pAttachments = colorAttachments.data(),
            .blendConstants = { 0.0F, 0.0F, 0.0F, 0.0F }
        }; 
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = NULL,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = NULL
        }; 
        
        VkResult layoutResult = vkCreatePipelineLayout( device->GetVulkanDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout );
        if ( layoutResult != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create pipeline layout {}", (int32)layoutResult );
        }

        VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .pNext = NULL,
            .viewMask = 0,
            .colorAttachmentCount = colorAttachmentCount,
            .pColorAttachmentFormats = pixelFormats.data(),
            .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED
        };

        VkGraphicsPipelineCreateInfo pipelineCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &pipelineRenderingCreateInfo,
            .flags = 0,
            .stageCount = (uint32)stages.size(),
            .pStages = stages.data(),
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pTessellationState = NULL,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depStencil,
            .pColorBlendState = &colorInfo,
            .pDynamicState = &dynamicState,
            .layout = pipelineLayout,
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0,
        };

        VkResult result = vkCreateGraphicsPipelines( device->GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &pipeline);
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create graphics pipeline {}", (int32)result );
        }
    }

    VulkanRHIGraphicsPipeline::~VulkanRHIGraphicsPipeline()
    {
    }
    
    bool VulkanRHIGraphicsPipeline::IsValid() const
    {
        return pipeline != VK_NULL_HANDLE;
    }

    VulkanRHI::~VulkanRHI()
    {
        delete GVulkanDevice;
        delete GVulkanInstance;
        SDL_Vulkan_UnloadLibrary();
    }

    EDynamicRHI VulkanRHI::GetType() const
    {
        return EDynamicRHI::Vulkan;
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

    VulkanRHI::VulkanRHI() : DynamicRHI()
    {
        VkApplicationInfo appInfo =
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = NULL,
            .pApplicationName = "Vulkan",
            .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
            .pEngineName = "Empty Engine",
            .engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
            .apiVersion = VK_API_VERSION_1_3
        };

        if ( SDL_Vulkan_LoadLibrary( nullptr ) != 0 )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to load SDL Vulkan Library! {}", Text::NarrowToWide( SDL_GetError() ) );
        }

        TArray<const NChar*> layers;
#ifdef EMPTYENGINE_CORE_LOG
        layers.emplace_back( "VK_LAYER_KHRONOS_validation" );
#endif

        uint32 extensionCount;
        const NChar* const* extensionNames = NULL;
        GetVulkanInstanceExtensions( &extensionCount, extensionNames );

        TArray<const NChar*> extensions;
        for ( uint32 i = 0; i < extensionCount; i++ )
        {
            extensions.emplace_back( extensionNames[ i ] );
        }
#ifdef EMPTYENGINE_CORE_LOG
        extensions.emplace_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
#endif

        VkInstanceCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = (uint32)layers.size(),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = (uint32)extensions.size(),
            .ppEnabledExtensionNames = extensions.data()
        };

        GVulkanInstance = new VulkanRHIInstance( createInfo );

#ifdef EMPTYENGINE_CORE_LOG
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
        vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( GVulkanInstance->GetVulkanInstance(), "vkCreateDebugUtilsMessengerEXT");
        if ( vkCreateDebugUtilsMessenger == VK_NULL_HANDLE )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to find function vkCreateDebugUtilsMessengerEXT" );
            return;
        }

        VkDebugUtilsMessengerCreateInfoEXT messageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = NULL,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = VulkanDebugCallback,
            .pUserData = NULL
        };

        VkResult result = vkCreateDebugUtilsMessenger( GVulkanInstance->GetVulkanInstance(), &messageCreateInfo, NULL, &GVulkanDebugMessager );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CORE_CRITICAL( L"Failed to create vulkan messenger {}", (uint32)result );
        }
#endif
    }

	void VulkanRHI::SetName( RHIResource* pResource, const NChar* name ) { }

	void VulkanRHI::WaitForDevice() const { }

    const RHIDevice* VulkanRHI::GetRHIDevice() const
    {
        return GVulkanDevice;
    }

    const WString& VulkanRHI::GetName() const
    {
        static WString const deviceName = L"Vulkan " 
            + Text::FormatUnit( VK_API_VERSION_MAJOR( VK_HEADER_VERSION_COMPLETE ), 0 ) + L'.'
            + Text::FormatUnit( VK_API_VERSION_MINOR( VK_HEADER_VERSION_COMPLETE ), 0 ) + L'.' 
            + Text::FormatUnit( VK_API_VERSION_PATCH( VK_HEADER_VERSION_COMPLETE ), 0 );
        return deviceName;
    }

    RHIPresentContext* VulkanRHI::CreateRHIPresentContext( Window* window ) const
    {
        return new VulkanRHIPresentContext( window, GVulkanInstance );
    }

    RHISurface* VulkanRHI::CreateRHISurface( Window* window ) const
    {
        return new VulkanRHISurface( window, GVulkanInstance );
    }

    RHICommandBuffer* VulkanRHI::CreateRHICommandBuffer( const RHICommandBufferCreateInfo& info ) const
    {
        return new VulkanRHICommandBuffer( info, GVulkanDevice, GVulkanDevice->GetGraphicsFamilyIndex() );
    }
    
    RHIBuffer* VulkanRHI::CreateRHIBuffer( const RHIBufferCreateInfo& info ) const
    {
        return NULL;
    }
    
    RHITexture* VulkanRHI::CreateRHITexture( const RHITextureCreateInfo& info ) const
    {
        return new VulkanRHITexture( info, GVulkanDevice );
    }
    
    RHISampler* VulkanRHI::CreateRHISampler( const RHISamplerCreateInfo& info ) const
    {
        return NULL;
    }
    
    RHIShaderStage* VulkanRHI::CreateRHIShaderStage( const RHIShaderStageCreateInfo& info ) const
    {
        return new VulkanRHIShaderStage( info, GVulkanDevice );
    }
    
    RHIGraphicsPipeline* VulkanRHI::CreateRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info ) const
    {
        return new VulkanRHIGraphicsPipeline( info, GVulkanDevice );
    }
}