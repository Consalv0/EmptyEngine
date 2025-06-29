
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

#include "Engine/Engine.h"
#include "Utils/Hasher.h"

#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/Vulkan.h"
#include <RHI/Vulkan/VulkanRHIObjects.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>

#include <set>
#include <map>

#ifdef EE_PLATFORM_WINDOWS
// --- Make discrete GPU by default.
extern "C" {
    // --- developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    // --- developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#endif

namespace EE
{
    static VkShaderStageFlagBits ConvertShaderStage( EShaderStageFlagsBit shaderStage )
    {
        switch ( shaderStage )
        {
        case ShaderStage_Tesselation_Bit:       return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage_Vertex_Bit:            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage_Geometry_Bit:          return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage_Fragment_Bit:          return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage_Compute_Bit:           return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            return VK_SHADER_STAGE_ALL;
        }
    }

    static VkShaderStageFlags ConvertShaderStageFlags( EShaderStageFlags shaderStages )
    {
        VkShaderStageFlags flags = 0;
        if ( shaderStages & ShaderStage_Tesselation_Bit )       flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        if ( shaderStages & ShaderStage_Vertex_Bit )            flags |= VK_SHADER_STAGE_VERTEX_BIT;
        if ( shaderStages & ShaderStage_Geometry_Bit )          flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
        if ( shaderStages & ShaderStage_Fragment_Bit )          flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
        if ( shaderStages & ShaderStage_Compute_Bit )           flags |= VK_SHADER_STAGE_COMPUTE_BIT;
        return flags;
    }

    static VkSampleCountFlagBits ConvertSampleCountBits( ESampleCountFlagsBit samples )
    {
        switch ( samples )
        {
            case SampleCount_1_Bit:  return VK_SAMPLE_COUNT_1_BIT;
            case SampleCount_2_Bit:  return VK_SAMPLE_COUNT_2_BIT;
            case SampleCount_4_Bit:  return VK_SAMPLE_COUNT_4_BIT;
            case SampleCount_8_Bit:  return VK_SAMPLE_COUNT_8_BIT;
            case SampleCount_16_Bit: return VK_SAMPLE_COUNT_16_BIT;
            case SampleCount_32_Bit: return VK_SAMPLE_COUNT_32_BIT;
            case SampleCount_64_Bit: return VK_SAMPLE_COUNT_64_BIT;
            default:
                return VK_SAMPLE_COUNT_1_BIT;
        }
    }

    static VkAttachmentLoadOp ConvertLoadAttachmentOperation( ETextureOperation operation )
    {
        switch ( operation )
        {
        case EE::TextureOperation_Clear:        return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case EE::TextureOperation_Store:        return VK_ATTACHMENT_LOAD_OP_LOAD;
        case EE::TextureOperation_DontCare:
        default:                                return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
    }

    static VkAttachmentStoreOp ConvertStoreAttachmentOperation( ETextureOperation operation )
    {
        switch ( operation )
        {
        case EE::TextureOperation_Store:        return VK_ATTACHMENT_STORE_OP_STORE;
        case EE::TextureOperation_DontCare:
        default:                                return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
    }

    static bool IsPixelFormatDepth( EPixelFormat format )
    {
        switch ( format )
        {
        case PixelFormat_D16_UNORM:
        case PixelFormat_X8_D24_UNORM_PACK32:
        case PixelFormat_D32_SFLOAT:
        case PixelFormat_S8_UINT:
        case PixelFormat_D16_UNORM_S8_UINT:
        case PixelFormat_D24_UNORM_S8_UINT:
        case PixelFormat_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
        }
    }

    static VkFormat ConvertPixelFormat( EPixelFormat format )
    {
        switch ( format )
        {
            case PixelFormat_R4G4_UNORM_PACK8:              return VK_FORMAT_R4G4_UNORM_PACK8;
            case PixelFormat_R4G4B4A4_UNORM_PACK16:         return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
            case PixelFormat_B4G4R4A4_UNORM_PACK16:         return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
            case PixelFormat_R5G6B5_UNORM_PACK16:           return VK_FORMAT_R5G6B5_UNORM_PACK16;
            case PixelFormat_B5G6R5_UNORM_PACK16:           return VK_FORMAT_B5G6R5_UNORM_PACK16;
            case PixelFormat_R5G5B5A1_UNORM_PACK16:         return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
            case PixelFormat_B5G5R5A1_UNORM_PACK16:         return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
            case PixelFormat_A1R5G5B5_UNORM_PACK16:         return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
            case PixelFormat_R8_UNORM:                      return VK_FORMAT_R8_UNORM;
            case PixelFormat_R8_SNORM:                      return VK_FORMAT_R8_SNORM;
            case PixelFormat_R8_USCALED:                    return VK_FORMAT_R8_USCALED;
            case PixelFormat_R8_SSCALED:                    return VK_FORMAT_R8_SSCALED;
            case PixelFormat_R8_UINT:                       return VK_FORMAT_R8_UINT;
            case PixelFormat_R8_SINT:                       return VK_FORMAT_R8_SINT;
            case PixelFormat_R8_SRGB:                       return VK_FORMAT_R8_SRGB;
            case PixelFormat_R8G8_UNORM:                    return VK_FORMAT_R8G8_UNORM;
            case PixelFormat_R8G8_SNORM:                    return VK_FORMAT_R8G8_SNORM;
            case PixelFormat_R8G8_USCALED:                  return VK_FORMAT_R8G8_USCALED;
            case PixelFormat_R8G8_SSCALED:                  return VK_FORMAT_R8G8_SSCALED;
            case PixelFormat_R8G8_UINT:                     return VK_FORMAT_R8G8_UINT;
            case PixelFormat_R8G8_SINT:                     return VK_FORMAT_R8G8_SINT;
            case PixelFormat_R8G8_SRGB:                     return VK_FORMAT_R8G8_SRGB;
            case PixelFormat_R8G8B8_UNORM:                  return VK_FORMAT_R8G8B8_UNORM;
            case PixelFormat_R8G8B8_SNORM:                  return VK_FORMAT_R8G8B8_SNORM;
            case PixelFormat_R8G8B8_USCALED:                return VK_FORMAT_R8G8B8_USCALED;
            case PixelFormat_R8G8B8_SSCALED:                return VK_FORMAT_R8G8B8_SSCALED;
            case PixelFormat_R8G8B8_UINT:                   return VK_FORMAT_R8G8B8_UINT;
            case PixelFormat_R8G8B8_SINT:                   return VK_FORMAT_R8G8B8_SINT;
            case PixelFormat_R8G8B8_SRGB:                   return VK_FORMAT_R8G8B8_SRGB;
            case PixelFormat_B8G8R8_UNORM:                  return VK_FORMAT_B8G8R8_UNORM;
            case PixelFormat_B8G8R8_SNORM:                  return VK_FORMAT_B8G8R8_SNORM;
            case PixelFormat_B8G8R8_USCALED:                return VK_FORMAT_B8G8R8_USCALED;
            case PixelFormat_B8G8R8_SSCALED:                return VK_FORMAT_B8G8R8_SSCALED;
            case PixelFormat_B8G8R8_UINT:                   return VK_FORMAT_B8G8R8_UINT;
            case PixelFormat_B8G8R8_SINT:                   return VK_FORMAT_B8G8R8_SINT;
            case PixelFormat_B8G8R8_SRGB:                   return VK_FORMAT_B8G8R8_SRGB;
            case PixelFormat_R8G8B8A8_UNORM:                return VK_FORMAT_R8G8B8A8_UNORM;
            case PixelFormat_R8G8B8A8_SNORM:                return VK_FORMAT_R8G8B8A8_SNORM;
            case PixelFormat_R8G8B8A8_USCALED:              return VK_FORMAT_R8G8B8A8_USCALED;
            case PixelFormat_R8G8B8A8_SSCALED:              return VK_FORMAT_R8G8B8A8_SSCALED;
            case PixelFormat_R8G8B8A8_UINT:                 return VK_FORMAT_R8G8B8A8_UINT;
            case PixelFormat_R8G8B8A8_SINT:                 return VK_FORMAT_R8G8B8A8_SINT;
            case PixelFormat_R8G8B8A8_SRGB:                 return VK_FORMAT_R8G8B8A8_SRGB;
            case PixelFormat_B8G8R8A8_UNORM:                return VK_FORMAT_B8G8R8A8_UNORM;
            case PixelFormat_B8G8R8A8_SNORM:                return VK_FORMAT_B8G8R8A8_SNORM;
            case PixelFormat_B8G8R8A8_USCALED:              return VK_FORMAT_B8G8R8A8_USCALED;
            case PixelFormat_B8G8R8A8_SSCALED:              return VK_FORMAT_B8G8R8A8_SSCALED;
            case PixelFormat_B8G8R8A8_UINT:                 return VK_FORMAT_B8G8R8A8_UINT;
            case PixelFormat_B8G8R8A8_SINT:                 return VK_FORMAT_B8G8R8A8_SINT;
            case PixelFormat_B8G8R8A8_SRGB:                 return VK_FORMAT_B8G8R8A8_SRGB;
            case PixelFormat_A8B8G8R8_UNORM_PACK32:         return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
            case PixelFormat_A8B8G8R8_SNORM_PACK32:         return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
            case PixelFormat_A8B8G8R8_USCALED_PACK32:       return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
            case PixelFormat_A8B8G8R8_SSCALED_PACK32:       return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
            case PixelFormat_A8B8G8R8_UINT_PACK32:          return VK_FORMAT_A8B8G8R8_UINT_PACK32;
            case PixelFormat_A8B8G8R8_SINT_PACK32:          return VK_FORMAT_A8B8G8R8_SINT_PACK32;
            case PixelFormat_A8B8G8R8_SRGB_PACK32:          return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
            case PixelFormat_A2R10G10B10_UNORM_PACK32:      return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
            case PixelFormat_A2R10G10B10_SNORM_PACK32:      return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
            case PixelFormat_A2R10G10B10_USCALED_PACK32:    return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
            case PixelFormat_A2R10G10B10_SSCALED_PACK32:    return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
            case PixelFormat_A2R10G10B10_UINT_PACK32:       return VK_FORMAT_A2R10G10B10_UINT_PACK32;
            case PixelFormat_A2R10G10B10_SINT_PACK32:       return VK_FORMAT_A2R10G10B10_SINT_PACK32;
            case PixelFormat_A2B10G10R10_UNORM_PACK32:      return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            case PixelFormat_A2B10G10R10_SNORM_PACK32:      return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
            case PixelFormat_A2B10G10R10_USCALED_PACK32:    return VK_FORMAT_A2B10G10R10_USCALED_PACK32;
            case PixelFormat_A2B10G10R10_SSCALED_PACK32:    return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
            case PixelFormat_A2B10G10R10_UINT_PACK32:       return VK_FORMAT_A2B10G10R10_UINT_PACK32;
            case PixelFormat_A2B10G10R10_SINT_PACK32:       return VK_FORMAT_A2B10G10R10_SINT_PACK32;
            case PixelFormat_R16_UNORM:                     return VK_FORMAT_R16_UNORM;
            case PixelFormat_R16_SNORM:                     return VK_FORMAT_R16_SNORM;
            case PixelFormat_R16_USCALED:                   return VK_FORMAT_R16_USCALED;
            case PixelFormat_R16_SSCALED:                   return VK_FORMAT_R16_SSCALED;
            case PixelFormat_R16_UINT:                      return VK_FORMAT_R16_UINT;
            case PixelFormat_R16_SINT:                      return VK_FORMAT_R16_SINT;
            case PixelFormat_R16_SFLOAT:                    return VK_FORMAT_R16_SFLOAT;
            case PixelFormat_R16G16_UNORM:                  return VK_FORMAT_R16G16_UNORM;
            case PixelFormat_R16G16_SNORM:                  return VK_FORMAT_R16G16_SNORM;
            case PixelFormat_R16G16_USCALED:                return VK_FORMAT_R16G16_USCALED;
            case PixelFormat_R16G16_SSCALED:                return VK_FORMAT_R16G16_SSCALED;
            case PixelFormat_R16G16_UINT:                   return VK_FORMAT_R16G16_UINT;
            case PixelFormat_R16G16_SINT:                   return VK_FORMAT_R16G16_SINT;
            case PixelFormat_R16G16_SFLOAT:                 return VK_FORMAT_R16G16_SFLOAT;
            case PixelFormat_R16G16B16_UNORM:               return VK_FORMAT_R16G16B16_UNORM;
            case PixelFormat_R16G16B16_SNORM:               return VK_FORMAT_R16G16B16_SNORM;
            case PixelFormat_R16G16B16_USCALED:             return VK_FORMAT_R16G16B16_USCALED;
            case PixelFormat_R16G16B16_SSCALED:             return VK_FORMAT_R16G16B16_SSCALED;
            case PixelFormat_R16G16B16_UINT:                return VK_FORMAT_R16G16B16_UINT;
            case PixelFormat_R16G16B16_SINT:                return VK_FORMAT_R16G16B16_SINT;
            case PixelFormat_R16G16B16_SFLOAT:              return VK_FORMAT_R16G16B16_SFLOAT;
            case PixelFormat_R16G16B16A16_UNORM:            return VK_FORMAT_R16G16B16A16_UNORM;
            case PixelFormat_R16G16B16A16_SNORM:            return VK_FORMAT_R16G16B16A16_SNORM;
            case PixelFormat_R16G16B16A16_USCALED:          return VK_FORMAT_R16G16B16A16_USCALED;
            case PixelFormat_R16G16B16A16_SSCALED:          return VK_FORMAT_R16G16B16A16_SSCALED;
            case PixelFormat_R16G16B16A16_UINT:             return VK_FORMAT_R16G16B16A16_UINT;
            case PixelFormat_R16G16B16A16_SINT:             return VK_FORMAT_R16G16B16A16_SINT;
            case PixelFormat_R16G16B16A16_SFLOAT:           return VK_FORMAT_R16G16B16A16_SFLOAT;
            case PixelFormat_R32_UINT:                      return VK_FORMAT_R32_UINT;
            case PixelFormat_R32_SINT:                      return VK_FORMAT_R32_SINT;
            case PixelFormat_R32_SFLOAT:                    return VK_FORMAT_R32_SFLOAT;
            case PixelFormat_R32G32_UINT:                   return VK_FORMAT_R32G32_UINT;
            case PixelFormat_R32G32_SINT:                   return VK_FORMAT_R32G32_SINT;
            case PixelFormat_R32G32_SFLOAT:                 return VK_FORMAT_R32G32_SFLOAT;
            case PixelFormat_R32G32B32_UINT:                return VK_FORMAT_R32G32B32_UINT;
            case PixelFormat_R32G32B32_SINT:                return VK_FORMAT_R32G32B32_SINT;
            case PixelFormat_R32G32B32_SFLOAT:              return VK_FORMAT_R32G32B32_SFLOAT;
            case PixelFormat_R32G32B32A32_UINT:             return VK_FORMAT_R32G32B32A32_UINT;
            case PixelFormat_R32G32B32A32_SINT:             return VK_FORMAT_R32G32B32A32_SINT;
            case PixelFormat_R32G32B32A32_SFLOAT:           return VK_FORMAT_R32G32B32A32_SFLOAT;
            case PixelFormat_R64_UINT:                      return VK_FORMAT_R64_UINT;
            case PixelFormat_R64_SINT:                      return VK_FORMAT_R64_SINT;
            case PixelFormat_R64_SFLOAT:                    return VK_FORMAT_R64_SFLOAT;
            case PixelFormat_R64G64_UINT:                   return VK_FORMAT_R64G64_UINT;
            case PixelFormat_R64G64_SINT:                   return VK_FORMAT_R64G64_SINT;
            case PixelFormat_R64G64_SFLOAT:                 return VK_FORMAT_R64G64_SFLOAT;
            case PixelFormat_R64G64B64_UINT:                return VK_FORMAT_R64G64B64_UINT;
            case PixelFormat_R64G64B64_SINT:                return VK_FORMAT_R64G64B64_SINT;
            case PixelFormat_R64G64B64_SFLOAT:              return VK_FORMAT_R64G64B64_SFLOAT;
            case PixelFormat_R64G64B64A64_UINT:             return VK_FORMAT_R64G64B64A64_UINT;
            case PixelFormat_R64G64B64A64_SINT:             return VK_FORMAT_R64G64B64A64_SINT;
            case PixelFormat_R64G64B64A64_SFLOAT:           return VK_FORMAT_R64G64B64A64_SFLOAT;
            case PixelFormat_B10G11R11_UFLOAT_PACK32:       return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
            case PixelFormat_E5B9G9R9_UFLOAT_PACK32:        return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
            case PixelFormat_D16_UNORM:                     return VK_FORMAT_D16_UNORM;
            case PixelFormat_X8_D24_UNORM_PACK32:           return VK_FORMAT_X8_D24_UNORM_PACK32;
            case PixelFormat_D32_SFLOAT:                    return VK_FORMAT_D32_SFLOAT;
            case PixelFormat_S8_UINT:                       return VK_FORMAT_S8_UINT;
            case PixelFormat_D16_UNORM_S8_UINT:             return VK_FORMAT_D16_UNORM_S8_UINT;
            case PixelFormat_D24_UNORM_S8_UINT:             return VK_FORMAT_D24_UNORM_S8_UINT;
            case PixelFormat_D32_SFLOAT_S8_UINT:            return VK_FORMAT_D32_SFLOAT_S8_UINT;
            case PixelFormat_BC1_RGB_UNORM_BLOCK:           return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
            case PixelFormat_BC1_RGB_SRGB_BLOCK:            return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
            case PixelFormat_BC1_RGBA_UNORM_BLOCK:          return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
            case PixelFormat_BC1_RGBA_SRGB_BLOCK:           return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
            case PixelFormat_BC2_UNORM_BLOCK:               return VK_FORMAT_BC2_UNORM_BLOCK;
            case PixelFormat_BC2_SRGB_BLOCK:                return VK_FORMAT_BC2_SRGB_BLOCK;
            case PixelFormat_BC3_UNORM_BLOCK:               return VK_FORMAT_BC3_UNORM_BLOCK;
            case PixelFormat_BC3_SRGB_BLOCK:                return VK_FORMAT_BC3_SRGB_BLOCK;
            case PixelFormat_BC4_UNORM_BLOCK:               return VK_FORMAT_BC4_UNORM_BLOCK;
            case PixelFormat_BC4_SNORM_BLOCK:               return VK_FORMAT_BC4_SNORM_BLOCK;
            case PixelFormat_BC5_UNORM_BLOCK:               return VK_FORMAT_BC5_UNORM_BLOCK;
            case PixelFormat_BC5_SNORM_BLOCK:               return VK_FORMAT_BC5_SNORM_BLOCK;
            case PixelFormat_BC6H_UFLOAT_BLOCK:             return VK_FORMAT_BC6H_UFLOAT_BLOCK;
            case PixelFormat_BC6H_SFLOAT_BLOCK:             return VK_FORMAT_BC6H_SFLOAT_BLOCK;
            case PixelFormat_BC7_UNORM_BLOCK:               return VK_FORMAT_BC7_UNORM_BLOCK;
            case PixelFormat_BC7_SRGB_BLOCK:                return VK_FORMAT_BC7_SRGB_BLOCK;
            case PixelFormat_ETC2_R8G8B8_UNORM_BLOCK:       return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
            case PixelFormat_ETC2_R8G8B8_SRGB_BLOCK:        return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
            case PixelFormat_ETC2_R8G8B8A1_UNORM_BLOCK:     return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
            case PixelFormat_ETC2_R8G8B8A1_SRGB_BLOCK:      return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
            case PixelFormat_ETC2_R8G8B8A8_UNORM_BLOCK:     return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
            case PixelFormat_ETC2_R8G8B8A8_SRGB_BLOCK:      return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
            case PixelFormat_EAC_R11_UNORM_BLOCK:           return VK_FORMAT_EAC_R11_UNORM_BLOCK;
            case PixelFormat_EAC_R11_SNORM_BLOCK:           return VK_FORMAT_EAC_R11_SNORM_BLOCK;
            case PixelFormat_EAC_R11G11_UNORM_BLOCK:        return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
            case PixelFormat_EAC_R11G11_SNORM_BLOCK:        return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
            case PixelFormat_ASTC_4x4_UNORM_BLOCK:          return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
            case PixelFormat_ASTC_4x4_SRGB_BLOCK:           return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
            case PixelFormat_ASTC_5x4_UNORM_BLOCK:          return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
            case PixelFormat_ASTC_5x4_SRGB_BLOCK:           return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
            case PixelFormat_ASTC_5x5_UNORM_BLOCK:          return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
            case PixelFormat_ASTC_5x5_SRGB_BLOCK:           return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
            case PixelFormat_ASTC_6x5_UNORM_BLOCK:          return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
            case PixelFormat_ASTC_6x5_SRGB_BLOCK:           return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
            case PixelFormat_ASTC_6x6_UNORM_BLOCK:          return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
            case PixelFormat_ASTC_6x6_SRGB_BLOCK:           return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
            case PixelFormat_ASTC_8x5_UNORM_BLOCK:          return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
            case PixelFormat_ASTC_8x5_SRGB_BLOCK:           return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
            case PixelFormat_ASTC_8x6_UNORM_BLOCK:          return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
            case PixelFormat_ASTC_8x6_SRGB_BLOCK:           return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
            case PixelFormat_ASTC_8x8_UNORM_BLOCK:          return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
            case PixelFormat_ASTC_8x8_SRGB_BLOCK:           return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
            case PixelFormat_ASTC_10x5_UNORM_BLOCK:         return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
            case PixelFormat_ASTC_10x5_SRGB_BLOCK:          return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
            case PixelFormat_ASTC_10x6_UNORM_BLOCK:         return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
            case PixelFormat_ASTC_10x6_SRGB_BLOCK:          return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
            case PixelFormat_ASTC_10x8_UNORM_BLOCK:         return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
            case PixelFormat_ASTC_10x8_SRGB_BLOCK:          return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
            case PixelFormat_ASTC_10x10_UNORM_BLOCK:        return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
            case PixelFormat_ASTC_10x10_SRGB_BLOCK:         return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
            case PixelFormat_ASTC_12x10_UNORM_BLOCK:        return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
            case PixelFormat_ASTC_12x10_SRGB_BLOCK:         return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
            case PixelFormat_ASTC_12x12_UNORM_BLOCK:        return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
            case PixelFormat_ASTC_12x12_SRGB_BLOCK:         return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
            case PixelFormat_A4R4G4B4_UNORM_PACK16:         return VK_FORMAT_A4R4G4B4_UNORM_PACK16;
            case PixelFormat_A4B4G4R4_UNORM_PACK16:         return VK_FORMAT_A4B4G4R4_UNORM_PACK16;
            case PixelFormat_ASTC_4x4_SFLOAT_BLOCK:         return VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK;
            case PixelFormat_ASTC_5x4_SFLOAT_BLOCK:         return VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK;
            case PixelFormat_ASTC_5x5_SFLOAT_BLOCK:         return VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK;
            case PixelFormat_ASTC_6x5_SFLOAT_BLOCK:         return VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK;
            case PixelFormat_ASTC_6x6_SFLOAT_BLOCK:         return VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK;
            case PixelFormat_ASTC_8x5_SFLOAT_BLOCK:         return VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK;
            case PixelFormat_ASTC_8x6_SFLOAT_BLOCK:         return VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK;
            case PixelFormat_ASTC_8x8_SFLOAT_BLOCK:         return VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK;
            case PixelFormat_ASTC_10x5_SFLOAT_BLOCK:        return VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK;
            case PixelFormat_ASTC_10x6_SFLOAT_BLOCK:        return VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK;
            case PixelFormat_ASTC_10x8_SFLOAT_BLOCK:        return VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK;
            case PixelFormat_ASTC_10x10_SFLOAT_BLOCK:       return VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK;
            case PixelFormat_ASTC_12x10_SFLOAT_BLOCK:       return VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK;
            case PixelFormat_ASTC_12x12_SFLOAT_BLOCK:       return VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK;
            case PixelFormat_PVRTC1_2BPP_UNORM_BLOCK_IMG:   return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
            case PixelFormat_PVRTC1_4BPP_UNORM_BLOCK_IMG:   return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
            case PixelFormat_PVRTC2_2BPP_UNORM_BLOCK_IMG:   return VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
            case PixelFormat_PVRTC2_4BPP_UNORM_BLOCK_IMG:   return VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
            case PixelFormat_PVRTC1_2BPP_SRGB_BLOCK_IMG:    return VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
            case PixelFormat_PVRTC1_4BPP_SRGB_BLOCK_IMG:    return VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
            case PixelFormat_PVRTC2_2BPP_SRGB_BLOCK_IMG:    return VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
            case PixelFormat_PVRTC2_4BPP_SRGB_BLOCK_IMG:    return VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
            case PixelFormat_R16G16_SFIXED5_NV:             return VK_FORMAT_R16G16_SFIXED5_NV;
            case PixelFormat_A1B5G5R5_UNORM_PACK16_KHR:     return VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR;
            case PixelFormat_A8_UNORM_KHR:                  return VK_FORMAT_A8_UNORM_KHR;
            case PixelFormatVideo_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:   return VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16;
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    static EPixelFormat ConvertImageFormat( VkFormat vkFormat )
    {
        switch ( vkFormat )
        {
            case VK_FORMAT_R4G4_UNORM_PACK8:              return PixelFormat_R4G4_UNORM_PACK8;
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16:         return PixelFormat_R4G4B4A4_UNORM_PACK16;
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16:         return PixelFormat_B4G4R4A4_UNORM_PACK16;
            case VK_FORMAT_R5G6B5_UNORM_PACK16:           return PixelFormat_R5G6B5_UNORM_PACK16;
            case VK_FORMAT_B5G6R5_UNORM_PACK16:           return PixelFormat_B5G6R5_UNORM_PACK16;
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16:         return PixelFormat_R5G5B5A1_UNORM_PACK16;
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16:         return PixelFormat_B5G5R5A1_UNORM_PACK16;
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16:         return PixelFormat_A1R5G5B5_UNORM_PACK16;
            case VK_FORMAT_R8_UNORM:                      return PixelFormat_R8_UNORM;
            case VK_FORMAT_R8_SNORM:                      return PixelFormat_R8_SNORM;
            case VK_FORMAT_R8_USCALED:                    return PixelFormat_R8_USCALED;
            case VK_FORMAT_R8_SSCALED:                    return PixelFormat_R8_SSCALED;
            case VK_FORMAT_R8_UINT:                       return PixelFormat_R8_UINT;
            case VK_FORMAT_R8_SINT:                       return PixelFormat_R8_SINT;
            case VK_FORMAT_R8_SRGB:                       return PixelFormat_R8_SRGB;
            case VK_FORMAT_R8G8_UNORM:                    return PixelFormat_R8G8_UNORM;
            case VK_FORMAT_R8G8_SNORM:                    return PixelFormat_R8G8_SNORM;
            case VK_FORMAT_R8G8_USCALED:                  return PixelFormat_R8G8_USCALED;
            case VK_FORMAT_R8G8_SSCALED:                  return PixelFormat_R8G8_SSCALED;
            case VK_FORMAT_R8G8_UINT:                     return PixelFormat_R8G8_UINT;
            case VK_FORMAT_R8G8_SINT:                     return PixelFormat_R8G8_SINT;
            case VK_FORMAT_R8G8_SRGB:                     return PixelFormat_R8G8_SRGB;
            case VK_FORMAT_R8G8B8_UNORM:                  return PixelFormat_R8G8B8_UNORM;
            case VK_FORMAT_R8G8B8_SNORM:                  return PixelFormat_R8G8B8_SNORM;
            case VK_FORMAT_R8G8B8_USCALED:                return PixelFormat_R8G8B8_USCALED;
            case VK_FORMAT_R8G8B8_SSCALED:                return PixelFormat_R8G8B8_SSCALED;
            case VK_FORMAT_R8G8B8_UINT:                   return PixelFormat_R8G8B8_UINT;
            case VK_FORMAT_R8G8B8_SINT:                   return PixelFormat_R8G8B8_SINT;
            case VK_FORMAT_R8G8B8_SRGB:                   return PixelFormat_R8G8B8_SRGB;
            case VK_FORMAT_B8G8R8_UNORM:                  return PixelFormat_B8G8R8_UNORM;
            case VK_FORMAT_B8G8R8_SNORM:                  return PixelFormat_B8G8R8_SNORM;
            case VK_FORMAT_B8G8R8_USCALED:                return PixelFormat_B8G8R8_USCALED;
            case VK_FORMAT_B8G8R8_SSCALED:                return PixelFormat_B8G8R8_SSCALED;
            case VK_FORMAT_B8G8R8_UINT:                   return PixelFormat_B8G8R8_UINT;
            case VK_FORMAT_B8G8R8_SINT:                   return PixelFormat_B8G8R8_SINT;
            case VK_FORMAT_B8G8R8_SRGB:                   return PixelFormat_B8G8R8_SRGB;
            case VK_FORMAT_R8G8B8A8_UNORM:                return PixelFormat_R8G8B8A8_UNORM;
            case VK_FORMAT_R8G8B8A8_SNORM:                return PixelFormat_R8G8B8A8_SNORM;
            case VK_FORMAT_R8G8B8A8_USCALED:              return PixelFormat_R8G8B8A8_USCALED;
            case VK_FORMAT_R8G8B8A8_SSCALED:              return PixelFormat_R8G8B8A8_SSCALED;
            case VK_FORMAT_R8G8B8A8_UINT:                 return PixelFormat_R8G8B8A8_UINT;
            case VK_FORMAT_R8G8B8A8_SINT:                 return PixelFormat_R8G8B8A8_SINT;
            case VK_FORMAT_R8G8B8A8_SRGB:                 return PixelFormat_R8G8B8A8_SRGB;
            case VK_FORMAT_B8G8R8A8_UNORM:                return PixelFormat_B8G8R8A8_UNORM;
            case VK_FORMAT_B8G8R8A8_SNORM:                return PixelFormat_B8G8R8A8_SNORM;
            case VK_FORMAT_B8G8R8A8_USCALED:              return PixelFormat_B8G8R8A8_USCALED;
            case VK_FORMAT_B8G8R8A8_SSCALED:              return PixelFormat_B8G8R8A8_SSCALED;
            case VK_FORMAT_B8G8R8A8_UINT:                 return PixelFormat_B8G8R8A8_UINT;
            case VK_FORMAT_B8G8R8A8_SINT:                 return PixelFormat_B8G8R8A8_SINT;
            case VK_FORMAT_B8G8R8A8_SRGB:                 return PixelFormat_B8G8R8A8_SRGB;
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32:         return PixelFormat_A8B8G8R8_UNORM_PACK32;
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32:         return PixelFormat_A8B8G8R8_SNORM_PACK32;
            case VK_FORMAT_A8B8G8R8_USCALED_PACK32:       return PixelFormat_A8B8G8R8_USCALED_PACK32;
            case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:       return PixelFormat_A8B8G8R8_SSCALED_PACK32;
            case VK_FORMAT_A8B8G8R8_UINT_PACK32:          return PixelFormat_A8B8G8R8_UINT_PACK32;
            case VK_FORMAT_A8B8G8R8_SINT_PACK32:          return PixelFormat_A8B8G8R8_SINT_PACK32;
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32:          return PixelFormat_A8B8G8R8_SRGB_PACK32;
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32:      return PixelFormat_A2R10G10B10_UNORM_PACK32;
            case VK_FORMAT_A2R10G10B10_SNORM_PACK32:      return PixelFormat_A2R10G10B10_SNORM_PACK32;
            case VK_FORMAT_A2R10G10B10_USCALED_PACK32:    return PixelFormat_A2R10G10B10_USCALED_PACK32;
            case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:    return PixelFormat_A2R10G10B10_SSCALED_PACK32;
            case VK_FORMAT_A2R10G10B10_UINT_PACK32:       return PixelFormat_A2R10G10B10_UINT_PACK32;
            case VK_FORMAT_A2R10G10B10_SINT_PACK32:       return PixelFormat_A2R10G10B10_SINT_PACK32;
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32:      return PixelFormat_A2B10G10R10_UNORM_PACK32;
            case VK_FORMAT_A2B10G10R10_SNORM_PACK32:      return PixelFormat_A2B10G10R10_SNORM_PACK32;
            case VK_FORMAT_A2B10G10R10_USCALED_PACK32:    return PixelFormat_A2B10G10R10_USCALED_PACK32;
            case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:    return PixelFormat_A2B10G10R10_SSCALED_PACK32;
            case VK_FORMAT_A2B10G10R10_UINT_PACK32:       return PixelFormat_A2B10G10R10_UINT_PACK32;
            case VK_FORMAT_A2B10G10R10_SINT_PACK32:       return PixelFormat_A2B10G10R10_SINT_PACK32;
            case VK_FORMAT_R16_UNORM:                     return PixelFormat_R16_UNORM;
            case VK_FORMAT_R16_SNORM:                     return PixelFormat_R16_SNORM;
            case VK_FORMAT_R16_USCALED:                   return PixelFormat_R16_USCALED;
            case VK_FORMAT_R16_SSCALED:                   return PixelFormat_R16_SSCALED;
            case VK_FORMAT_R16_UINT:                      return PixelFormat_R16_UINT;
            case VK_FORMAT_R16_SINT:                      return PixelFormat_R16_SINT;
            case VK_FORMAT_R16_SFLOAT:                    return PixelFormat_R16_SFLOAT;
            case VK_FORMAT_R16G16_UNORM:                  return PixelFormat_R16G16_UNORM;
            case VK_FORMAT_R16G16_SNORM:                  return PixelFormat_R16G16_SNORM;
            case VK_FORMAT_R16G16_USCALED:                return PixelFormat_R16G16_USCALED;
            case VK_FORMAT_R16G16_SSCALED:                return PixelFormat_R16G16_SSCALED;
            case VK_FORMAT_R16G16_UINT:                   return PixelFormat_R16G16_UINT;
            case VK_FORMAT_R16G16_SINT:                   return PixelFormat_R16G16_SINT;
            case VK_FORMAT_R16G16_SFLOAT:                 return PixelFormat_R16G16_SFLOAT;
            case VK_FORMAT_R16G16B16_UNORM:               return PixelFormat_R16G16B16_UNORM;
            case VK_FORMAT_R16G16B16_SNORM:               return PixelFormat_R16G16B16_SNORM;
            case VK_FORMAT_R16G16B16_USCALED:             return PixelFormat_R16G16B16_USCALED;
            case VK_FORMAT_R16G16B16_SSCALED:             return PixelFormat_R16G16B16_SSCALED;
            case VK_FORMAT_R16G16B16_UINT:                return PixelFormat_R16G16B16_UINT;
            case VK_FORMAT_R16G16B16_SINT:                return PixelFormat_R16G16B16_SINT;
            case VK_FORMAT_R16G16B16_SFLOAT:              return PixelFormat_R16G16B16_SFLOAT;
            case VK_FORMAT_R16G16B16A16_UNORM:            return PixelFormat_R16G16B16A16_UNORM;
            case VK_FORMAT_R16G16B16A16_SNORM:            return PixelFormat_R16G16B16A16_SNORM;
            case VK_FORMAT_R16G16B16A16_USCALED:          return PixelFormat_R16G16B16A16_USCALED;
            case VK_FORMAT_R16G16B16A16_SSCALED:          return PixelFormat_R16G16B16A16_SSCALED;
            case VK_FORMAT_R16G16B16A16_UINT:             return PixelFormat_R16G16B16A16_UINT;
            case VK_FORMAT_R16G16B16A16_SINT:             return PixelFormat_R16G16B16A16_SINT;
            case VK_FORMAT_R16G16B16A16_SFLOAT:           return PixelFormat_R16G16B16A16_SFLOAT;
            case VK_FORMAT_R32_UINT:                      return PixelFormat_R32_UINT;
            case VK_FORMAT_R32_SINT:                      return PixelFormat_R32_SINT;
            case VK_FORMAT_R32_SFLOAT:                    return PixelFormat_R32_SFLOAT;
            case VK_FORMAT_R32G32_UINT:                   return PixelFormat_R32G32_UINT;
            case VK_FORMAT_R32G32_SINT:                   return PixelFormat_R32G32_SINT;
            case VK_FORMAT_R32G32_SFLOAT:                 return PixelFormat_R32G32_SFLOAT;
            case VK_FORMAT_R32G32B32_UINT:                return PixelFormat_R32G32B32_UINT;
            case VK_FORMAT_R32G32B32_SINT:                return PixelFormat_R32G32B32_SINT;
            case VK_FORMAT_R32G32B32_SFLOAT:              return PixelFormat_R32G32B32_SFLOAT;
            case VK_FORMAT_R32G32B32A32_UINT:             return PixelFormat_R32G32B32A32_UINT;
            case VK_FORMAT_R32G32B32A32_SINT:             return PixelFormat_R32G32B32A32_SINT;
            case VK_FORMAT_R32G32B32A32_SFLOAT:           return PixelFormat_R32G32B32A32_SFLOAT;
            case VK_FORMAT_R64_UINT:                      return PixelFormat_R64_UINT;
            case VK_FORMAT_R64_SINT:                      return PixelFormat_R64_SINT;
            case VK_FORMAT_R64_SFLOAT:                    return PixelFormat_R64_SFLOAT;
            case VK_FORMAT_R64G64_UINT:                   return PixelFormat_R64G64_UINT;
            case VK_FORMAT_R64G64_SINT:                   return PixelFormat_R64G64_SINT;
            case VK_FORMAT_R64G64_SFLOAT:                 return PixelFormat_R64G64_SFLOAT;
            case VK_FORMAT_R64G64B64_UINT:                return PixelFormat_R64G64B64_UINT;
            case VK_FORMAT_R64G64B64_SINT:                return PixelFormat_R64G64B64_SINT;
            case VK_FORMAT_R64G64B64_SFLOAT:              return PixelFormat_R64G64B64_SFLOAT;
            case VK_FORMAT_R64G64B64A64_UINT:             return PixelFormat_R64G64B64A64_UINT;
            case VK_FORMAT_R64G64B64A64_SINT:             return PixelFormat_R64G64B64A64_SINT;
            case VK_FORMAT_R64G64B64A64_SFLOAT:           return PixelFormat_R64G64B64A64_SFLOAT;
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32:       return PixelFormat_B10G11R11_UFLOAT_PACK32;
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:        return PixelFormat_E5B9G9R9_UFLOAT_PACK32;
            case VK_FORMAT_D16_UNORM:                     return PixelFormat_D16_UNORM;
            case VK_FORMAT_X8_D24_UNORM_PACK32:           return PixelFormat_X8_D24_UNORM_PACK32;
            case VK_FORMAT_D32_SFLOAT:                    return PixelFormat_D32_SFLOAT;
            case VK_FORMAT_S8_UINT:                       return PixelFormat_S8_UINT;
            case VK_FORMAT_D16_UNORM_S8_UINT:             return PixelFormat_D16_UNORM_S8_UINT;
            case VK_FORMAT_D24_UNORM_S8_UINT:             return PixelFormat_D24_UNORM_S8_UINT;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:            return PixelFormat_D32_SFLOAT_S8_UINT;
            case VK_FORMAT_BC1_RGB_UNORM_BLOCK:           return PixelFormat_BC1_RGB_UNORM_BLOCK;
            case VK_FORMAT_BC1_RGB_SRGB_BLOCK:            return PixelFormat_BC1_RGB_SRGB_BLOCK;
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:          return PixelFormat_BC1_RGBA_UNORM_BLOCK;
            case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:           return PixelFormat_BC1_RGBA_SRGB_BLOCK;
            case VK_FORMAT_BC2_UNORM_BLOCK:               return PixelFormat_BC2_UNORM_BLOCK;
            case VK_FORMAT_BC2_SRGB_BLOCK:                return PixelFormat_BC2_SRGB_BLOCK;
            case VK_FORMAT_BC3_UNORM_BLOCK:               return PixelFormat_BC3_UNORM_BLOCK;
            case VK_FORMAT_BC3_SRGB_BLOCK:                return PixelFormat_BC3_SRGB_BLOCK;
            case VK_FORMAT_BC4_UNORM_BLOCK:               return PixelFormat_BC4_UNORM_BLOCK;
            case VK_FORMAT_BC4_SNORM_BLOCK:               return PixelFormat_BC4_SNORM_BLOCK;
            case VK_FORMAT_BC5_UNORM_BLOCK:               return PixelFormat_BC5_UNORM_BLOCK;
            case VK_FORMAT_BC5_SNORM_BLOCK:               return PixelFormat_BC5_SNORM_BLOCK;
            case VK_FORMAT_BC6H_UFLOAT_BLOCK:             return PixelFormat_BC6H_UFLOAT_BLOCK;
            case VK_FORMAT_BC6H_SFLOAT_BLOCK:             return PixelFormat_BC6H_SFLOAT_BLOCK;
            case VK_FORMAT_BC7_UNORM_BLOCK:               return PixelFormat_BC7_UNORM_BLOCK;
            case VK_FORMAT_BC7_SRGB_BLOCK:                return PixelFormat_BC7_SRGB_BLOCK;
            case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:       return PixelFormat_ETC2_R8G8B8_UNORM_BLOCK;
            case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:        return PixelFormat_ETC2_R8G8B8_SRGB_BLOCK;
            case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:     return PixelFormat_ETC2_R8G8B8A1_UNORM_BLOCK;
            case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:      return PixelFormat_ETC2_R8G8B8A1_SRGB_BLOCK;
            case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:     return PixelFormat_ETC2_R8G8B8A8_UNORM_BLOCK;
            case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:      return PixelFormat_ETC2_R8G8B8A8_SRGB_BLOCK;
            case VK_FORMAT_EAC_R11_UNORM_BLOCK:           return PixelFormat_EAC_R11_UNORM_BLOCK;
            case VK_FORMAT_EAC_R11_SNORM_BLOCK:           return PixelFormat_EAC_R11_SNORM_BLOCK;
            case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:        return PixelFormat_EAC_R11G11_UNORM_BLOCK;
            case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:        return PixelFormat_EAC_R11G11_SNORM_BLOCK;
            case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:          return PixelFormat_ASTC_4x4_UNORM_BLOCK;
            case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:           return PixelFormat_ASTC_4x4_SRGB_BLOCK;
            case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:          return PixelFormat_ASTC_5x4_UNORM_BLOCK;
            case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:           return PixelFormat_ASTC_5x4_SRGB_BLOCK;
            case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:          return PixelFormat_ASTC_5x5_UNORM_BLOCK;
            case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:           return PixelFormat_ASTC_5x5_SRGB_BLOCK;
            case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:          return PixelFormat_ASTC_6x5_UNORM_BLOCK;
            case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:           return PixelFormat_ASTC_6x5_SRGB_BLOCK;
            case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:          return PixelFormat_ASTC_6x6_UNORM_BLOCK;
            case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:           return PixelFormat_ASTC_6x6_SRGB_BLOCK;
            case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:          return PixelFormat_ASTC_8x5_UNORM_BLOCK;
            case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:           return PixelFormat_ASTC_8x5_SRGB_BLOCK;
            case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:          return PixelFormat_ASTC_8x6_UNORM_BLOCK;
            case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:           return PixelFormat_ASTC_8x6_SRGB_BLOCK;
            case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:          return PixelFormat_ASTC_8x8_UNORM_BLOCK;
            case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:           return PixelFormat_ASTC_8x8_SRGB_BLOCK;
            case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:         return PixelFormat_ASTC_10x5_UNORM_BLOCK;
            case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:          return PixelFormat_ASTC_10x5_SRGB_BLOCK;
            case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:         return PixelFormat_ASTC_10x6_UNORM_BLOCK;
            case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:          return PixelFormat_ASTC_10x6_SRGB_BLOCK;
            case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:         return PixelFormat_ASTC_10x8_UNORM_BLOCK;
            case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:          return PixelFormat_ASTC_10x8_SRGB_BLOCK;
            case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:        return PixelFormat_ASTC_10x10_UNORM_BLOCK;
            case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:         return PixelFormat_ASTC_10x10_SRGB_BLOCK;
            case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:        return PixelFormat_ASTC_12x10_UNORM_BLOCK;
            case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:         return PixelFormat_ASTC_12x10_SRGB_BLOCK;
            case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:        return PixelFormat_ASTC_12x12_UNORM_BLOCK;
            case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:         return PixelFormat_ASTC_12x12_SRGB_BLOCK;
            case VK_FORMAT_A4R4G4B4_UNORM_PACK16:         return PixelFormat_A4R4G4B4_UNORM_PACK16;
            case VK_FORMAT_A4B4G4R4_UNORM_PACK16:         return PixelFormat_A4B4G4R4_UNORM_PACK16;
            case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:         return PixelFormat_ASTC_4x4_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:         return PixelFormat_ASTC_5x4_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:         return PixelFormat_ASTC_5x5_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:         return PixelFormat_ASTC_6x5_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:         return PixelFormat_ASTC_6x6_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:         return PixelFormat_ASTC_8x5_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:         return PixelFormat_ASTC_8x6_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:         return PixelFormat_ASTC_8x8_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:        return PixelFormat_ASTC_10x5_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:        return PixelFormat_ASTC_10x6_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:        return PixelFormat_ASTC_10x8_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:       return PixelFormat_ASTC_10x10_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:       return PixelFormat_ASTC_12x10_SFLOAT_BLOCK;
            case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:       return PixelFormat_ASTC_12x12_SFLOAT_BLOCK;
            case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:   return PixelFormat_PVRTC1_2BPP_UNORM_BLOCK_IMG;
            case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:   return PixelFormat_PVRTC1_4BPP_UNORM_BLOCK_IMG;
            case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:   return PixelFormat_PVRTC2_2BPP_UNORM_BLOCK_IMG;
            case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:   return PixelFormat_PVRTC2_4BPP_UNORM_BLOCK_IMG;
            case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:    return PixelFormat_PVRTC1_2BPP_SRGB_BLOCK_IMG;
            case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:    return PixelFormat_PVRTC1_4BPP_SRGB_BLOCK_IMG;
            case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:    return PixelFormat_PVRTC2_2BPP_SRGB_BLOCK_IMG;
            case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:    return PixelFormat_PVRTC2_4BPP_SRGB_BLOCK_IMG;
            case VK_FORMAT_R16G16_SFIXED5_NV:             return PixelFormat_R16G16_SFIXED5_NV;
            case VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR:     return PixelFormat_A1B5G5R5_UNORM_PACK16_KHR;
            case VK_FORMAT_A8_UNORM_KHR:                  return PixelFormat_A8_UNORM_KHR;
            case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:  return  PixelFormatVideo_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16;
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
        case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:       return ColorSpace_Linear;
        case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:          return ColorSpace_Linear;
        case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:           return ColorSpace_sRGB;
        case VK_COLOR_SPACE_BT709_LINEAR_EXT:               return ColorSpace_Linear;
        case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:            return ColorSpace_sRGB;
        case VK_COLOR_SPACE_BT2020_LINEAR_EXT:              return ColorSpace_Linear;
        case VK_COLOR_SPACE_HDR10_ST2084_EXT:               return ColorSpace_HDR;
        case VK_COLOR_SPACE_DOLBYVISION_EXT:                return ColorSpace_HDR;
        case VK_COLOR_SPACE_HDR10_HLG_EXT:                  return ColorSpace_HDR;
        case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:            return ColorSpace_Linear;
        case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:         return ColorSpace_sRGB;
        case VK_COLOR_SPACE_PASS_THROUGH_EXT:               return ColorSpace_Unknown;
        case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:    return ColorSpace_sRGB;
        case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD:             return ColorSpace_Unknown;
        default:
            return ColorSpace_Unknown;
        }
    }

    static VkFormat ConvertVertexFormat( EVertexFormat format )
    {
        switch ( format )
        {
        case VertexFormat_UINT_8X2:     return VK_FORMAT_R8G8_UINT;
        case VertexFormat_UINT_8X4:     return VK_FORMAT_R8G8B8A8_UINT;
        case VertexFormat_SINT_8X2:     return VK_FORMAT_R8G8_SINT;
        case VertexFormat_SINT_8X4:     return VK_FORMAT_R8G8B8A8_SINT;
        case VertexFormat_UNORM_8X2:    return VK_FORMAT_R8G8_UNORM;
        case VertexFormat_UNORM_8X4:    return VK_FORMAT_R8G8B8A8_UNORM;
        case VertexFormat_SNORM_8X2:    return VK_FORMAT_R8G8_SNORM;
        case VertexFormat_SNORM_8X4:    return VK_FORMAT_R8G8B8A8_SNORM;
        case VertexFormat_UINT_16X2:    return VK_FORMAT_R16G16_UINT;
        case VertexFormat_UINT_16X4:    return VK_FORMAT_R16G16B16A16_UINT;
        case VertexFormat_SINT_16X2:    return VK_FORMAT_R16G16_SINT;
        case VertexFormat_SINT_16X4:    return VK_FORMAT_R16G16B16A16_SINT;
        case VertexFormat_UNORM_16X2:   return VK_FORMAT_R16G16_UNORM;
        case VertexFormat_UNORM_16X4:   return VK_FORMAT_R16G16B16A16_UNORM;
        case VertexFormat_SNORM_16X2:   return VK_FORMAT_R16G16_SNORM;
        case VertexFormat_SNORM_16X4:   return VK_FORMAT_R16G16B16A16_SNORM;
        case VertexFormat_FLOAT_16X2:   return VK_FORMAT_R16G16_SFLOAT;
        case VertexFormat_FLOAT_16X4:   return VK_FORMAT_R16G16B16A16_SFLOAT;
        case VertexFormat_FLOAT_32X1:   return VK_FORMAT_R32_SFLOAT;
        case VertexFormat_FLOAT_32X2:   return VK_FORMAT_R32G32_SFLOAT;
        case VertexFormat_FLOAT_32X3:   return VK_FORMAT_R32G32B32_SFLOAT;
        case VertexFormat_FLOAT_32X4:   return VK_FORMAT_R32G32B32A32_SFLOAT;
        case VertexFormat_UINT_32X1:    return VK_FORMAT_R32_UINT;
        case VertexFormat_UINT_32X2:    return VK_FORMAT_R32G32_UINT;
        case VertexFormat_UINT_32X3:    return VK_FORMAT_R32G32B32_UINT;
        case VertexFormat_UINT_32X4:    return VK_FORMAT_R32G32B32A32_UINT;
        case VertexFormat_SINT_32X1:    return VK_FORMAT_R32_SINT;
        case VertexFormat_SINT_32X2:    return VK_FORMAT_R32G32_SINT;
        case VertexFormat_SINT_32X3:    return VK_FORMAT_R32G32B32_SINT;
        case VertexFormat_SINT_32X4:    return VK_FORMAT_R32G32B32A32_SINT;
        case VertexFormat_Unknown:
        default:
            return VK_FORMAT_UNDEFINED;
        }
    }

    static VkVertexInputRate ConvertVertexStepMode( EVertexStepMode mode )
    {
        switch ( mode )
        {
        case VertexStepMode_Vertex:     return VK_VERTEX_INPUT_RATE_VERTEX;
        case VertexStepMode_Instance:   return VK_VERTEX_INPUT_RATE_INSTANCE;
        default:
            return VK_VERTEX_INPUT_RATE_VERTEX;
        }
    }

    static VkPresentModeKHR ConvertPresentMode( EPresentMode type )
    {
        switch ( type )
        {
        case PresentMode_Inmediate:     return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case PresentMode_VSync:         return VK_PRESENT_MODE_FIFO_KHR;
        default:
            return VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    static VkImageTiling ConvertTextureTiling( ETilingMode tiling )
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

    static VkImageViewType ConvertTextureType( ETextureType type )
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

    static VkImageUsageFlags ConvertTextureUsage( EUsageModeFlags usage )
    {
        VkImageUsageFlags flags = VkImageUsageFlags( 0 );
        if ( usage & UsageMode_Sampled_Bit )    flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if ( usage & UsageMode_Transfer_Bit )   flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if ( usage & UsageMode_Storage_Bit )    flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        if ( usage & UsageMode_Color_Bit )      flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if ( usage & UsageMode_DepthStencil )   flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        return flags;
    }

    static VkImageAspectFlags ConvertTextureAspect( ETextureAspectFlags aspect )
    {
        VkImageAspectFlags flags = VK_IMAGE_ASPECT_NONE;
        if ( aspect & TextureAspect_Color_Bit )     flags |= VK_IMAGE_ASPECT_COLOR_BIT;
        if ( aspect & TextureAspect_Depth_Bit )     flags |= VK_IMAGE_ASPECT_DEPTH_BIT;
        if ( aspect & TextureAspect_Stencil_Bit )   flags |= VK_IMAGE_ASPECT_STENCIL_BIT;
        return flags;
    }

    static VkImageLayout ConvertTextureLayout( ETextureLayout layout )
    {
        switch ( layout )
        {
        case TextureLayout_Undefined:           return VK_IMAGE_LAYOUT_UNDEFINED;
        case TextureLayout_General:             return VK_IMAGE_LAYOUT_GENERAL;
        case TextureLayout_TransferSRC:         return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case TextureLayout_TransferDST:         return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case TextureLayout_Shader:              return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case TextureLayout_ColorAttachment:     return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case TextureLayout_DepthAttachment:     return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        case TextureLayout_StencilAttachment:   return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case TextureLayout_Present:             return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        default:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    static VkSharingMode ConvertSharingMode( ESharingMode sharing )
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

    static VkBufferUsageFlags ConvertBufferUsageFlags( EBufferUsageFlags usages )
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

    static VkColorComponentFlags ConvertColorComponentFlags( EColorComponentFlags components )
    {
        VkColorComponentFlags flags = 0;
        if ( components & ColorComponent_R_Bit )     flags |= VK_COLOR_COMPONENT_R_BIT;
        if ( components & ColorComponent_G_Bit )     flags |= VK_COLOR_COMPONENT_G_BIT;
        if ( components & ColorComponent_B_Bit )     flags |= VK_COLOR_COMPONENT_B_BIT;
        if ( components & ColorComponent_A_Bit )     flags |= VK_COLOR_COMPONENT_A_BIT;
        return flags;
    }

    static VkPipelineStageFlags ConvertPipelineStageFlags( EPipelineStage stage )
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
        case FaceWinding_Clockwise:        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case FaceWinding_CounterClockwise: return VK_FRONT_FACE_CLOCKWISE;
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

    static VkBorderColor ConvertSamplerBorder( ESamplerBorder border )
    {

        switch ( border )
        {
        case SamplerBorder_Zero:    return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case SamplerBorder_Black:   return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case SamplerBorder_White:   return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        default:
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        }
    }

    static VkSamplerAddressMode ConvertSamplerMode( ESamplerMode sampler )
    {
        switch ( sampler )
        {
        case SamplerMode_Repeat:   return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerMode_Mirror:   return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case SamplerMode_Border:   return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case SamplerMode_Clamp:    return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }

    static VkFilter ConvertMinFilterMode( EFilterMode filter )
    {
        switch ( filter )
        {
        case FilterMode_MinMagNearest:          return VK_FILTER_NEAREST;
        case FilterMode_MinMagLinear:           return VK_FILTER_LINEAR;
        case FilterMode_MinLinearMagNearest:    return VK_FILTER_LINEAR;
        case FilterMode_MinNearestMagLinear:    return VK_FILTER_NEAREST;
        default:
            return VK_FILTER_NEAREST;
        }
    }

    static VkFilter ConvertMagFilterMode( EFilterMode filter )
    {
        switch ( filter )
        {
        case FilterMode_MinMagNearest:          return VK_FILTER_NEAREST;
        case FilterMode_MinMagLinear:           return VK_FILTER_LINEAR;
        case FilterMode_MinLinearMagNearest:    return VK_FILTER_NEAREST;
        case FilterMode_MinNearestMagLinear:    return VK_FILTER_LINEAR;
        default:
            return VK_FILTER_NEAREST;
        }
    }

    static VkCompareOp ConvertCompareOperation( ECompareOperation operation )
    {
        switch ( operation )
        {
        case CompareOperation_Never:        return VK_COMPARE_OP_NEVER;
        case CompareOperation_Less:         return VK_COMPARE_OP_LESS;
        case CompareOperation_Equal:        return VK_COMPARE_OP_EQUAL;
        case CompareOperation_LessEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOperation_Greater:      return VK_COMPARE_OP_GREATER;
        case CompareOperation_NotEqual:     return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOperation_GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOperation_Always:       return VK_COMPARE_OP_ALWAYS;
        default:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        }
    }

    static VkStencilOp ConvertStencilOperation( EStencilOperation operation )
    {
        switch ( operation )
        {
        case StencilOperation_Keep:             return VK_STENCIL_OP_KEEP;
        case StencilOperation_Zero:             return VK_STENCIL_OP_ZERO;
        case StencilOperation_Replace:          return VK_STENCIL_OP_REPLACE;
        case StencilOperation_IncrementClamp:   return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case StencilOperation_DecrementClamp:   return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case StencilOperation_Invert:           return VK_STENCIL_OP_INVERT;
        case StencilOperation_IncrementWrap:    return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case StencilOperation_DecrementWrap:    return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            return VK_STENCIL_OP_MAX_ENUM;
        }
    }

    static VkDescriptorType ConvertDescriptorType( EBindingType type )
    {
        switch ( type )
        {
        case BindingType_Uniform:           return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case BindingType_Storage:           return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case BindingType_UniformDynamic:    return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        case BindingType_StorageDynamic:    return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        case BindingType_Sampler:           return VK_DESCRIPTOR_TYPE_SAMPLER;
        case BindingType_TextureView:       return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case BindingType_TextureStorage:    return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        default:
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
        }
    }

    static const U8Char* GetVKResultString( VkResult result )
    {
        switch ( result )
        {
        case VK_SUCCESS:                                            return "VK_SUCCESS";
        case VK_NOT_READY:                                          return "VK_NOT_READY";
        case VK_TIMEOUT:                                            return "VK_TIMEOUT";
        case VK_EVENT_SET:                                          return "VK_EVENT_SET";
        case VK_EVENT_RESET:                                        return "VK_EVENT_RESET";
        case VK_INCOMPLETE:                                         return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:                           return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:                         return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:                        return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:                                  return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:                            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:                            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:                        return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:                          return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:                          return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:                             return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:                         return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:                              return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:                                      return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:                           return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:                      return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:                                return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:               return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED:                          return "VK_PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_SURFACE_LOST_KHR:                             return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:                     return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:                                     return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:                              return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:                     return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:                        return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:                            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:                return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:       return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:    return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:       return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:        return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:          return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_NOT_PERMITTED_KHR:                            return "VK_ERROR_NOT_PERMITTED_KHR";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:          return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR:                                    return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR:                                    return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR:                             return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR:                         return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:             return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:                    return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        case VK_INCOMPATIBLE_SHADER_BINARY_EXT:                     return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
        default: return "Unkown";
        }
    }

    static bool CreateNativeVmaAllocator( VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, VmaAllocator* outAllocator )
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
}

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
            EE_LOG_ERROR( "Vulkan: {}", callbackData->pMessage );
            return VK_FALSE;
        }
        if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
        {
            EE_LOG_WARN( "Vulkan: {}", callbackData->pMessage );
            return VK_FALSE;
        }
        if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT )
        {
            EE_LOG_INFO( "Vulkan: {}", callbackData->pMessage );
            return VK_FALSE;
        }
        if ( messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT )
        {
            EE_LOG_INFO( "Vulkan: {}", callbackData->pMessage );
            return VK_FALSE;
        }
        return VK_FALSE;
    }
#endif

    const TArray<const U8Char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const float QueuePriorities[] = { 1.0F };

    VulkanRHIPhysicalDevice::VulkanRHIPhysicalDevice( VkInstance instance, VkPhysicalDevice physicalDevice ) :
        instance_( instance ),
        physicalDevice_( physicalDevice )
    {
        vkGetPhysicalDeviceProperties( physicalDevice_, &deviceProperties_ );
        vkGetPhysicalDeviceFeatures( physicalDevice_, &deviceFeatures_ );

        // Logic to find queue family indices to populate struct with
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice_, &queueFamilyCount_, NULL );
        queueFamilies_.resize( queueFamilyCount_ );
        vkGetPhysicalDeviceQueueFamilyProperties( physicalDevice_, &queueFamilyCount_, queueFamilies_.data() );

        // Get extension support
        vkEnumerateDeviceExtensionProperties( physicalDevice_, NULL, &extensionCount_, NULL );
        extensions_.resize( extensionCount_ );
        vkEnumerateDeviceExtensionProperties( physicalDevice_, NULL, &extensionCount_, extensions_.data() );
    }

    VulkanRHIPhysicalDevice::~VulkanRHIPhysicalDevice()
    {

    }

    bool VulkanRHIPhysicalDevice::CheckDeviceExtensionSupport() const
    {
        std::set<std::string> requiredExtensions( DeviceExtensions.begin(), DeviceExtensions.end() );

        for ( const auto& extension : extensions_ )
        {
            requiredExtensions.erase( extension.extensionName );
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices VulkanRHIPhysicalDevice::GetQueueFamilies() const
    {
        QueueFamilyIndices indices;

        int i = 0;
        for ( const auto& queueFamily : queueFamilies_ )
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
        if ( auto search = surfaceSupportDetails_.find( surface->GetVulkanSurface() ); search != surfaceSupportDetails_.end() )
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
        if ( deviceProperties_.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
        {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties_.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if ( deviceFeatures_.geometryShader == VK_FALSE )
        {
            return 0;
        }

        if ( IsDeviceSuitable( surface ) == false )
        {
            return 0;
        }

        EE_LOG_INFO(
            "\u2570\u2500> {0} : {1}",
            deviceProperties_.deviceName,
            score
        );

        return score;
    }

    void VulkanRHIPhysicalDevice::AddSurfaceSupportDetails( VkSurfaceKHR surface, uint32 queueFamilyIndex )
    {
        VulkanSurfaceSupportDetails surfaceDetails;

        vkGetPhysicalDeviceSurfaceSupportKHR( physicalDevice_, queueFamilyIndex, surface, &surfaceDetails.supported );

        surfaceSupportDetails_.insert( std::make_pair( surface, surfaceDetails ) );

        if ( surfaceDetails.supported == VK_FALSE )
        {
            return;
        }

        surfaceSupportDetails_.insert( std::make_pair( surface, surfaceDetails ) );

        UpdateSurfaceSupportDetails( surface );
    }

    void VulkanRHIPhysicalDevice::UpdateSurfaceSupportDetails( VkSurfaceKHR surface )
    {
        if ( auto search = surfaceSupportDetails_.find( surface ); search != surfaceSupportDetails_.end() )
        {
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( physicalDevice_, surface, &search->second.capabilities );

            vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice_, surface, &search->second.formatCount, VK_NULL_HANDLE );
            if ( search->second.formatCount != 0 )
            {
                search->second.formats.resize( search->second.formatCount );
                vkGetPhysicalDeviceSurfaceFormatsKHR( physicalDevice_, surface, &search->second.formatCount, search->second.formats.data() );
            }

            vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice_, surface, &search->second.presentModeCount, VK_NULL_HANDLE );
            if ( search->second.presentModeCount != 0 )
            {
                search->second.presentModes.resize( search->second.presentModeCount );
                vkGetPhysicalDeviceSurfacePresentModesKHR( physicalDevice_, surface, &search->second.presentModeCount, search->second.presentModes.data() );
            }
        }
    };

    const VulkanSurfaceSupportDetails* VulkanRHIPhysicalDevice::GetSurfaceSupportDetails( VkSurfaceKHR surface ) const
    {
        if ( auto search = surfaceSupportDetails_.find( surface ); search != surfaceSupportDetails_.end() )
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
        VkResult createResult = vkCreateInstance( &createInfo, VK_NULL_HANDLE, &instance );
        if ( createResult != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "vkCreateInstance failed : {}", GetVKResultString( createResult ) );
            return;
        }

        // Get physical devices
        vkEnumeratePhysicalDevices( instance, &physicalDeviceCount, VK_NULL_HANDLE );
        if ( physicalDeviceCount == 0 )
        {
            EE_LOG_CRITICAL( "Failed to find GPUs with Vulkan support!" );
            return;
        }

        EE_LOG_INFO( "\u250C> Available devices : {}", physicalDeviceCount );

        // Get physical device info
        TArray<VkPhysicalDevice> vulkanPhysicalDevices( physicalDeviceCount );
        vkEnumeratePhysicalDevices( instance, &physicalDeviceCount, vulkanPhysicalDevices.data() );

        for ( uint32 i = 0; i < physicalDeviceCount; i++ )
        {
            VkPhysicalDevice& vulkanPhysicalDevice = vulkanPhysicalDevices[ i ];
            physicalDevices.push_back( new VulkanRHIPhysicalDevice( instance, vulkanPhysicalDevice ) );
        }
    }

    VulkanRHIInstance::~VulkanRHIInstance()
    {
        vkDestroyInstance( instance, VK_NULL_HANDLE );
        for ( uint32 i = 0; i < physicalDeviceCount; i++ )
        {
            delete physicalDevices[ i ];
        }
    }

    int32 VulkanRHIInstance::PickPhysicalDeviceForSurface( VulkanRHISurface* surface ) const
    {
        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, int> candidates;

        for ( uint32 i = 0; i < physicalDeviceCount; i++ )
        {
            int score = physicalDevices[ i ]->RateDeviceSuitabilityForSurface( surface );
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
            EE_LOG_CRITICAL( "Failed to find a suitable device!" );
            return false;
        }

        selectedDeviceIndex = selection;

        return true;
    }

    const VulkanRHIPhysicalDevice* VulkanRHIInstance::GetSelectedPhysicalDevice() const
    {
        return physicalDevices[ selectedDeviceIndex ];
    };

    VulkanRHIPhysicalDevice* VulkanRHIInstance::GetSelectedPhysicalDevice()
    {
        return physicalDevices[ selectedDeviceIndex ];
    };

    bool VulkanRHIInstance::AddSurfaceSupportDetails( VulkanRHISurface* surface )
    {
        for ( uint32 i = 0; i < physicalDeviceCount; i++ )
        {
            auto& internalPhysicalDevice = physicalDevices[ i ];
            internalPhysicalDevice->AddSurfaceSupportDetails( surface->GetVulkanSurface(), internalPhysicalDevice->GetQueueFamilies().graphicsFamily.value() );
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
        presentQueue( NULL ), graphicsQueue( NULL ), deviceLimits()
    {
        EE_ASSERT( GVulkanDevice == NULL, "Creating a second device!, Aborting..." );

        auto properties = physicalDevice->GetProperties();
        deviceLimits.minUniformBufferOffsetAlignment = properties.limits.minUniformBufferOffsetAlignment;
        deviceLimits.maxUniformBufferRange = properties.limits.maxUniformBufferRange;

        // Specifying the queues to be created
        QueueFamilyIndices indices = physicalDevice->GetQueueFamilies();

        TArray<VkDeviceQueueCreateInfo> queueCreateInfos;
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

        // VkPhysicalDeviceVulkan13Features deviceFeatures13
        // {
        //     .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        //     .dynamicRendering = VK_TRUE
        // };

        VkPhysicalDeviceFeatures deviceFeatures
        {
            .geometryShader = VK_TRUE,
            .sampleRateShading = VK_TRUE,
            .logicOp = VK_FALSE,
            .samplerAnisotropy = VK_TRUE,
        };

        VkDeviceCreateInfo deviceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = NULL, // &deviceFeatures13,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0,         //   deprecated?
            .ppEnabledLayerNames = NULL,    //   deprecated?
            .enabledExtensionCount = (uint32)DeviceExtensions.size(),
            .ppEnabledExtensionNames = DeviceExtensions.data(),
            .pEnabledFeatures = &deviceFeatures
        };

        EE_ASSERT
        (
            vkCreateDevice( physicalDevice->GetPhysicalDevice(), &deviceCreateInfo, nullptr, &device ) == VK_SUCCESS,
            "Failed to create logical device!"
        );

        EE_ASSERT
        (
            CreateNativeVmaAllocator( instance->GetVulkanInstance(), physicalDevice->GetPhysicalDevice(), device, &allocator ) == VK_SUCCESS,
            "Failed to create VMA Allocator!"
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
        vkDeviceWaitIdle( GVulkanDevice->GetVulkanDevice() );

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
        if ( success == false || isSwapChainDirty_ )
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
            EE_LOG_CRITICAL( "Failed to present image: {}! {}", swapChain->BackImageIndex(), GetVKResultString( result )  );
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

    const RHITextureView* VulkanRHIPresentContext::GetBackbuffer() const
    {
        return swapChain->GetTextureView( swapChain->BackImageIndex() );
    }

    void VulkanRHIPresentContext::UpdateSelectedSurfaceFormat()
    {
        auto& surfaceDetails = GVulkanDevice->GetVulkanPhysicalDevice()->GetSurfaceDetails( surface->GetVulkanSurface() );

        {
            const VkSurfaceFormatKHR& surfaceFormat = surfaceDetails.formats[ 0 ];
            EColorSpace colorSpace = ConvertColorSpace( surfaceFormat.colorSpace );
            EPixelFormat format = ConvertImageFormat( surfaceFormat.format );
            selectedSurfaceFormat_.pixelFormat = format;
            selectedSurfaceFormat_.colorSpace = colorSpace;
        }

        EPixelFormat desiredPixelFormat = window->GetDesiredPixelFormat();
        EColorSpace desiredColorSpace = window->GetDesiredColorSpace();
        if ( window->HDREnabled() == false )
        {
            if ( window->GetDesiredColorSpace() == ColorSpace_HDR )
            {
                desiredColorSpace = ColorSpace_sRGB;
            }
        }

        if ( desiredColorSpace == ColorSpace_Unknown && desiredPixelFormat == PixelFormat_Unknown )
        {
            return;
        }

        // Find best format, the window desides that
        bool contains = false;
        for ( uint32 i = 0; i < surfaceDetails.formatCount; i++ )
        {
            const VkSurfaceFormatKHR& surfaceFormat = surfaceDetails.formats[ i ];
            EColorSpace colorSpace = ConvertColorSpace( surfaceFormat.colorSpace );
            EPixelFormat format = ConvertImageFormat( surfaceFormat.format );

            if ( colorSpace == ColorSpace_Unknown )
                continue;
            if ( format == PixelFormat_Unknown )
                continue;

            if ((desiredColorSpace == colorSpace && desiredPixelFormat == PixelFormat_Unknown) ||
                (desiredColorSpace == ColorSpace_Unknown && desiredPixelFormat == format) ||
                (desiredColorSpace == colorSpace && desiredPixelFormat == format) )
            {
                selectedSurfaceFormat_.pixelFormat = format;
                selectedSurfaceFormat_.colorSpace = colorSpace;
                break;
            }
        }
    }

    void VulkanRHIPresentContext::SetSwapChainDirty()
    {
        isSwapChainDirty_ = true;
    }

    void VulkanRHIPresentContext::CreateSurface()
    {
        surface = new VulkanRHISurface( window, instance );

        // Vulkan needs the surface information in order to determine the driver, so the driver creation is here, the device selection is handled inside CreateSurface
        if ( GVulkanDevice == NULL )
        {
            if ( instance->SelectSuitableDevice( surface ) == false )
            {
                return;
            }

            GVulkanDevice = new VulkanRHIDevice( instance );
        }
        UpdateSelectedSurfaceFormat();
    }

    void VulkanRHIPresentContext::CreateSwapChain()
    {
        const RHISurfaceFormat& surfaceFormat = GetSurfaceFormat();

        RHISwapChainCreateInfo swapChainInfo
        {
            .window = this->window,
            .width = (uint32)window->GetWidth(),
            .height = (uint32)window->GetHeight(),
            .bufferCount = 2,
            .format = surfaceFormat.pixelFormat,
            .colorSpace = surfaceFormat.colorSpace,
            .presentMode = window->GetPresentMode(),
            .compositeAlpha = window->IsUsingCompositeAlpha(),
        };

        swapChain = new VulkanRHISwapChain( swapChainInfo, this, GVulkanDevice );
        isSwapChainDirty_ = false;
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
        UpdateSelectedSurfaceFormat();
        const RHISurfaceFormat& surfaceFormat = GetSurfaceFormat();

        RHISwapChainCreateInfo swapChainInfo
        {
            .window = window,
            .width = (uint32)window->GetWidth(),
            .height = (uint32)window->GetHeight(),
            .bufferCount = 2,
            .format = surfaceFormat.pixelFormat,
            .colorSpace = surfaceFormat.colorSpace,
            .presentMode = window->GetPresentMode(),
            .compositeAlpha = window->IsUsingCompositeAlpha(),
        };

        swapChain->CreateSwapChain( swapChainInfo );
        isSwapChainDirty_ = false;
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
        TArray<VkSemaphore> waitSemaphores;
        TArray<VkPipelineStageFlags> waitStageFlags;
        waitSemaphores.resize( waitSemaphoreCount );
        waitStageFlags.resize( waitSemaphoreCount );
        for ( uint32 i = 0; i < waitSemaphoreCount; i++ )
        {
            auto* vkSemaphore = static_cast<const VulkanRHISemaphore*>(info.waitSemaphores[ i ]);
            waitSemaphores[ i ] = vkSemaphore->GetVulkanSemaphore();
            waitStageFlags[ i ] = ConvertPipelineStageFlags( info.stageFlags );
        }

        uint32 signalSemaphoreCount = (uint32)info.signalSemaphores.size();
        TArray<VkSemaphore> signalSemaphores;
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
        VkResult result = vkQueueSubmit( queue, 1, &vkSubmitInfo, nativeFence );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed command vkQueueSubmit! {}", GetVKResultString( result ) );
        }
    }

    VulkanRHIBuffer::~VulkanRHIBuffer()
    {
        if ( buffer != VK_NULL_HANDLE )
        {
            vmaDestroyBuffer( device->GetVulkanAllocator(), buffer, NULL );
        }
    }

    VulkanRHIBuffer::VulkanRHIBuffer( const RHIBufferCreateInfo& info, VulkanRHIDevice* device ) : RHIBuffer()
        , device( device )
        , usage( info.usage )
        , sharing( info.sharing )
        , buffer( VK_NULL_HANDLE )
        , size( info.size )
        , aligment( info.aligment == 0 ? info.size : info.aligment )
        , offset( info.offset )
    {
        VkBufferCreateInfo bufferInfo = 
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .size = size,
            .usage = ConvertBufferUsageFlags( usage ),
        };

        if ( device->GetGraphicsFamilyIndex() != device->GetPresentFamilyIndex() && sharing == SharingMode_Dynamic )
        {
            bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
            bufferInfo.queueFamilyIndexCount = 2;
            bufferInfo.pQueueFamilyIndices = device->GetFamilyIndices();
        }
        else
        {
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            bufferInfo.queueFamilyIndexCount = 1;
            bufferInfo.pQueueFamilyIndices = device->GetFamilyIndices();
        }
        
        VkResult createResult = vkCreateBuffer( device->GetVulkanDevice(), &bufferInfo, nullptr, &buffer );
        if ( createResult != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create buffer! {}", GetVKResultString( createResult ) );
        }

        VmaAllocationCreateInfo allocInfo
        {
            .flags = 0,
            .usage = VMA_MEMORY_USAGE_AUTO,
            .requiredFlags = 0,
            .preferredFlags = 0,
            .memoryTypeBits = 0,
            .pool = NULL,
            .priority = 0
        };
        if ( (usage & BufferUsage_MapWite_Bit) > 0 )
        {
            allocInfo.flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }
        
        VkResult memoryResult = vmaCreateBuffer( device->GetVulkanAllocator(), &bufferInfo, &allocInfo, &buffer, &nativeAllocation, VK_NULL_HANDLE );
        if ( memoryResult != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to allocate buffer memory! {}", GetVKResultString( memoryResult ) );
        }
    }

    bool VulkanRHIBuffer::IsValid() const
    {
        return buffer != VK_NULL_HANDLE;
    }

    uint64 VulkanRHIBuffer::GetSize() const
    {
        return size;
    }

    uint64 VulkanRHIBuffer::GetOffset() const
    {
        return offset;
    }

    uint64 VulkanRHIBuffer::GetAligment() const
    {
        return aligment;
    }

    void VulkanRHIBuffer::UploadData( const void* data, uint64 offset, uint64 size ) const
    {
        void* gpuData = NULL;
        VkResult result = vmaMapMemory( device->GetVulkanAllocator(), nativeAllocation, &gpuData);
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to open memory map for buffer {}!", (int32)result );
            return;
        }

        memcpy( static_cast<char*>( gpuData ) + offset, data, size );

        vmaUnmapMemory( device->GetVulkanAllocator(), nativeAllocation );
    }

    void VulkanRHIBuffer::BindBuffer() const
    {
        VkResult result = vmaBindBufferMemory( device->GetVulkanAllocator(), nativeAllocation, buffer );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to bind buffer memory {}!", (int32)result );
        }
    }

    VulkanRHISampler::VulkanRHISampler( const RHISamplerCreateInfo& info, VulkanRHIDevice* device )
        : device_( device )
        , sampler_( VK_NULL_HANDLE )
        , filter_( info.filter )
        , modeU_( info.modeU ), modeV_( info.modeV ), modeW_( info.modeW )
        , border_( info.border )
    {
        VkSamplerCreateInfo samplerInfo
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .magFilter = ConvertMagFilterMode( filter_ ),
            .minFilter = ConvertMinFilterMode( filter_ ),
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = ConvertSamplerMode( modeU_ ),
            .addressModeV = ConvertSamplerMode( modeV_ ),
            .addressModeW = ConvertSamplerMode( modeW_ ),
            .mipLodBias = 0.0F,
            // Enable anisotropic filtering
            // This feature is optional, so we must check if it's supported on the device
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 1.0F,
            .compareEnable = VK_FALSE,
            .compareOp = ConvertCompareOperation( info.compareOperation ),
            .minLod = 0.0F,
            .maxLod = 0.0F,
            .borderColor = ConvertSamplerBorder( border_ ),
            .unnormalizedCoordinates = VK_FALSE
        };
        VkResult result = vkCreateSampler( device_->GetVulkanDevice(), &samplerInfo, NULL, &sampler_ );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create sampler! {}", GetVKResultString( result ) );
        }
    }

    VulkanRHISampler::~VulkanRHISampler()
    {
        vkDestroySampler( device_->GetVulkanDevice(), sampler_, NULL );
    }

    bool VulkanRHISampler::IsValid() const
    {
        return sampler_ != VK_NULL_HANDLE;
    }

    VulkanRHITextureView::VulkanRHITextureView( const RHITextureViewCreateInfo& info, VulkanRHIDevice* device ) :
        device_( device ),
        type_( info.type ),
        imageView_( VK_NULL_HANDLE )
    {
        subresourceRange_ = info.subresource;
        vulkanTexture_ = static_cast<const VulkanRHITexture*>(info.texture);

        VkImageViewCreateInfo viewInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .image = vulkanTexture_->GetVulkanImage(),
            .viewType = ConvertTextureType( type_ ),
            .format = ConvertPixelFormat( vulkanTexture_->GetFormat() ),
            .components = VkComponentMapping
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY
            },
            .subresourceRange = VkImageSubresourceRange
            {
                .aspectMask = ConvertTextureAspect( subresourceRange_.aspect ),
                .baseMipLevel = subresourceRange_.baseMipLevel,
                .levelCount = subresourceRange_.levelCount,
                .baseArrayLayer = subresourceRange_.baseArrayLayer,
                .layerCount = subresourceRange_.arrayLayer
            }
        };

        VkResult result = vkCreateImageView( device->GetVulkanDevice(), &viewInfo, nullptr, &imageView_ );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create texture image view! {}", GetVKResultString( result ) );
        }
    }

    VulkanRHITextureView::~VulkanRHITextureView()
    {
        if ( imageView_ != VK_NULL_HANDLE )
        {
            vkDestroyImageView( device_->GetVulkanDevice(), imageView_, NULL );
        }
    }
    
    bool VulkanRHITextureView::IsValid() const
    {
        return imageView_ != VK_NULL_HANDLE;
    }

    VulkanRHITexture::VulkanRHITexture( const RHITextureCreateInfo& info, VulkanRHIDevice* device, VkImage image ) :
        device_( device ),
        extents_( info.extents ),
        format_( ConvertPixelFormat( info.format ) ),
        pixelFormat_( ConvertImageFormat( format_ ) ),
        memory_( VK_NULL_HANDLE ),
        image_( image ),
        ownership_( false )
    {
        pixelFormat_ = ConvertImageFormat( format_ );
    }

    VulkanRHITexture::VulkanRHITexture( const RHITextureCreateInfo& info, VulkanRHIDevice* device ) :
        device_( device ),
        extents_( info.extents ),
        format_( ConvertPixelFormat( info.format ) ),
        pixelFormat_( ConvertImageFormat( format_ ) ),
        memory_( VK_NULL_HANDLE ), image_( VK_NULL_HANDLE ),
        ownership_( true )
    {
        VkImageCreateInfo imageInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format_,
            .extent
            {
                .width = extents_.x,
                .height = extents_.y,
                .depth = extents_.z
            },
            .mipLevels = info.mipLevels,
            .arrayLayers = info.arraySize,
            .samples = ConvertSampleCountBits( info.sampleCount ),
            .tiling = ConvertTextureTiling( info.tiling ),
            .usage = ConvertTextureUsage( info.usage ),
            .sharingMode = ConvertSharingMode( info.sharing ),
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = device->GetFamilyIndices(),
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VkResult imageResult = vkCreateImage( device->GetVulkanDevice(), &imageInfo, VK_NULL_HANDLE, &image_ );
        if ( imageResult != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create image! {}", GetVKResultString( imageResult ) );
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

        VkResult memoryResult = vmaCreateImage( device->GetVulkanAllocator(), &imageInfo, &allocInfo, &image_, &memory_, NULL );
        if ( memoryResult != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to allocate image memory! {}", GetVKResultString( memoryResult ) );
            return;
        }
    }

    VulkanRHITexture::~VulkanRHITexture()
    {
        if ( image_ != VK_NULL_HANDLE && ownership_ )
        {
            vkDestroyImage( device_->GetVulkanDevice(), image_, NULL );
        }
    }

    bool VulkanRHITexture::IsValid() const
    {
        return image_ != VK_NULL_HANDLE;
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
                    break;
                }
            }
        }

        if ( contains == false )
        {
            EE_LOG_CRITICAL( "Surface format {}, {} is not supported!", (uint32)info.format, (uint32)info.colorSpace );
            return;
        }

        uint32 width = info.width;
        uint32 height = info.height;
        // width = EE_CLAMP( width, surfaceDetails.capabilities.minImageExtent.width, surfaceDetails.capabilities.maxImageExtent.width );
        // height = EE_CLAMP( height, surfaceDetails.capabilities.minImageExtent.height, surfaceDetails.capabilities.maxImageExtent.height );
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
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE
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

        VkCompositeAlphaFlagBitsKHR compositeAlpha = (info.compositeAlpha ? VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
        if ( (surfaceDetails.capabilities.supportedCompositeAlpha & compositeAlpha) > 0 )
        {
            createInfo.compositeAlpha = compositeAlpha;
        }
        else
        {
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            EE_LOG_ERROR( "Falied to create surface with composite alpha, valid formats {}", (uint32)surfaceDetails.capabilities.supportedCompositeAlpha );
        }

        createInfo.preTransform = surfaceDetails.capabilities.currentTransform;
        createInfo.presentMode = ConvertPresentMode( info.presentMode );
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR( device->GetVulkanDevice(), &createInfo, NULL, &swapChain );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed vkCreateSwapchainKHR! {}", GetVKResultString( result ) );
        }

        vkGetSwapchainImagesKHR( device->GetVulkanDevice(), swapChain, &imageCount, NULL );
        images.resize( imageCount );
        vkGetSwapchainImagesKHR( device->GetVulkanDevice(), swapChain, &imageCount, images.data() );

        for ( uint32 i = 0; i < imageCount; i++ )
        {
            RHITextureCreateInfo textureCreateInfo
            {
                .type = TextureType_Texture2D,
                .extents = 
                {
                    width,
                    height,
                    1
                },
                .arraySize = 1,
                .mipLevels = 1,
                .format = info.format,
                .colorSpace = info.colorSpace
            };
            const VulkanRHITexture* texture = textures.emplace_back( new VulkanRHITexture( textureCreateInfo, device, images[ i ] ) );

            RHITextureViewCreateInfo textureViewCreateInfo
            {
                .type = TextureType_Texture2D,
                .layout = TextureLayout_ColorAttachment,
                .texture = texture,
                .subresource = RHITextureSubresource
                {
                    .aspect = TextureAspect_Color_Bit
                }
            };

            textureViews.emplace_back( new VulkanRHITextureView( textureViewCreateInfo, device ) );
        }
    }

    void VulkanRHISwapChain::Cleanup()
    {
        for ( VulkanRHITexture* texture : textures )
        {
            delete texture;
        }
        textures.clear();
        for ( VulkanRHITextureView* textureView : textureViews )
        {
            delete textureView;
        }
        textureViews.clear();

        vkDestroySwapchainKHR( device->GetVulkanDevice(), swapChain, NULL );
    }

    bool VulkanRHISwapChain::AquireNextImage( uint64 timeout, const VulkanRHISemaphore* semaphore, const VulkanRHIFence* fence )
    {
        VkSemaphore vulkanSemaphore = semaphore == NULL ? VK_NULL_HANDLE : semaphore->GetVulkanSemaphore();
        VkFence vulkanFence = fence == NULL ? VK_NULL_HANDLE : fence->GetVulkanFence();

        VkResult result = vkAcquireNextImageKHR( device->GetVulkanDevice(), swapChain, timeout, vulkanSemaphore, VK_NULL_HANDLE, &backImageIndex );
        if ( result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR )
        {
            EE_LOG_INFO( "Swap chain out of date!" );
            return false;
        }
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed vkAcquireNextImageKHR! {}", GetVKResultString( result ) );
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
        if ( SDL_Vulkan_CreateSurface( (SDL_Window*)window->GetWindowHandle(), instance->GetVulkanInstance(), VK_NULL_HANDLE, &surface ) == false )
        {
            EE_LOG_CRITICAL( "Failed SDL_Vulkan_CreateSurface! {}", SDL_GetError() );
            return;
        }

        instance->AddSurfaceSupportDetails( this );
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
            EE_LOG_CRITICAL( "Failed to fence: {}", GetVKResultString( createResult ) );
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
            EE_LOG_CRITICAL( "Failed reset fence: {}", GetVKResultString( result ) );
        }
    }

    void VulkanRHIFence::Wait( uint64 timeout ) const
    {
        auto result = vkWaitForFences( device->GetVulkanDevice(), 1, &fence, VK_TRUE, timeout );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed wait fence: {}", GetVKResultString( result ) );
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
            EE_LOG_CRITICAL( "Failed to create semaphore: {}", GetVKResultString( createResult ) );
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

    bool VulkanRHICommandPool::IsValid() const
    {
        return commandPool != VK_NULL_HANDLE;
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
            EE_LOG_CRITICAL( "Failed to create command pool: {}", GetVKResultString( createResult ) );
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
            EE_LOG_CRITICAL( "Unable to allocate command buffer {}", GetVKResultString( result ) );
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
            EE_LOG_CRITICAL( "Failed to end command buffer {}", GetVKResultString( result ) );
        }
    }

    void VulkanRHICommandBuffer::End() const
    {
        auto result = vkEndCommandBuffer( commandBuffer );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to end command buffer {}", GetVKResultString( result ) );
        }
    }

    void VulkanRHICommandBuffer::BindGraphicsPipeline( const RHIGraphicsPipeline* pipeline ) const
    {
        const VulkanRHIGraphicsPipeline* vulkanPipeline = static_cast<const VulkanRHIGraphicsPipeline*>( pipeline );
        vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVulkanPipeline() );
    }

    void VulkanRHICommandBuffer::BindBindingsGroup( const RHIGraphicsPipeline* pipeline, const RHIBindGroup* bindGroup, const uint32& dynamicOffsetsCount, const uint32* dynamicOffsets ) const
    {
        const VulkanRHIBindGroup* vulkanBindGroup = static_cast<const VulkanRHIBindGroup*>( bindGroup );
        const VulkanRHIGraphicsPipeline* vulkanPipeline = static_cast<const VulkanRHIGraphicsPipeline*>(pipeline);
        vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVulkanPipelineLayout(), 0, 1, &vulkanBindGroup->GetVulkanDescriptorSet(), dynamicOffsetsCount, dynamicOffsets );
    }

    void VulkanRHICommandBuffer::BindVertexBuffer( const RHIBuffer* buffer, uint32 bindIndex, uint64 offset ) const
    {
        const VulkanRHIBuffer* vulkanBuffer = static_cast<const VulkanRHIBuffer*>( buffer );
        vkCmdBindVertexBuffers( commandBuffer, bindIndex, 1, &vulkanBuffer->GetVulkanBuffer(), &offset );
    }

    void VulkanRHICommandBuffer::BindIndexBuffer( const RHIBuffer* buffer, uint64 offset ) const
    {
        const VulkanRHIBuffer* vulkanBuffer = static_cast<const VulkanRHIBuffer*>(buffer);
        vkCmdBindIndexBuffer( commandBuffer, vulkanBuffer->GetVulkanBuffer(), offset, VK_INDEX_TYPE_UINT32 );
    }

    void VulkanRHICommandBuffer::SetCullMode( const ECullModeFlags& cull ) const
    {
        vkCmdSetCullMode( commandBuffer, ConvertCullMode( cull ) );
    }

    void VulkanRHICommandBuffer::SetFrontFace( const EFaceWinding& frontFace ) const
    {
        vkCmdSetFrontFace( commandBuffer, ConvertFrontFace( frontFace ) );
    }

    void VulkanRHICommandBuffer::SetViewport( Box2f viewport, float minDepth, float maxDepth ) const
    {
        VkViewport vkviewport
        {
            .x = viewport.minX,
            .y = viewport.minY,
            .width = viewport.maxX,
            .height = viewport.maxY,
            .minDepth = minDepth,
            .maxDepth = maxDepth,
        };
        vkCmdSetViewport( commandBuffer, 0, 1, &vkviewport );
    }

    void VulkanRHICommandBuffer::SetScissor( IntBox2 scissor ) const
    {
        VkRect2D vkscissor
        {
            .offset = { scissor.minX, scissor.minX },
            .extent = { (uint32)scissor.maxX, (uint32)scissor.maxY }
        };
        vkCmdSetScissor( commandBuffer, 0, 1, &vkscissor );
    }

    void VulkanRHICommandBuffer::TransitionTexture( const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer, const ETextureLayout& from, const ETextureLayout& to ) const
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

    void VulkanRHICommandBuffer::CopyBuffer( const RHIBuffer* fromBuffer, const RHIBuffer* toBuffer, uint32 regionCount, const RHIBufferCopyRegion* copyRegions ) const
    {
        auto vulkanFromBuffer = static_cast<const VulkanRHIBuffer*>(fromBuffer);
        auto vulkanToBuffer = static_cast<const VulkanRHIBuffer*>(toBuffer);

        TArray<VkBufferCopy> regions( regionCount );

        for ( uint32 i = 0; i < regionCount; i++ )
        {
            const RHIBufferCopyRegion& region = copyRegions[ i ];
            regions[ i ] = VkBufferCopy
            {
                .srcOffset = region.sourceOffset,
                .dstOffset = region.destOffset,
                .size = region.size,
            };
        }

        vkCmdCopyBuffer
        (
            commandBuffer,
            vulkanFromBuffer->GetVulkanBuffer(),
            vulkanToBuffer->GetVulkanBuffer(),
            regionCount,
            regions.data()
        );
    }

    void VulkanRHICommandBuffer::CopyBufferToTexture( const RHIBuffer* buffer, const RHITexture* texture, const ETextureLayout& layout, uint32 regionCount, const RHIBufferImageCopyRegion* copyRegions ) const
    {
        auto vulkanBuffer = static_cast<const VulkanRHIBuffer*>(buffer);
        auto vulkanTexture = static_cast<const VulkanRHITexture*>(texture);

        TArray<VkBufferImageCopy> regions( regionCount );

        for ( uint32 i = 0; i < regionCount; i++ )
        {
            const RHIBufferImageCopyRegion& region = copyRegions[ i ];
            regions[ i ] = VkBufferImageCopy
            {
                .bufferOffset = region.bufferOffset,
                .bufferRowLength = region.bufferRowLength,
                .bufferImageHeight = region.bufferImageHeight,
                .imageSubresource
                {
                    .aspectMask = ConvertTextureAspect( region.subresource.aspect ),
                    .mipLevel = region.subresource.baseMipLevel,
                    .baseArrayLayer = region.subresource.baseArrayLayer,
                    .layerCount = region.subresource.arrayLayer
                },
                .imageOffset
                {
                    region.imageOffset.x,
                    region.imageOffset.y,
                    region.imageOffset.z
                },
                .imageExtent
                {
                    .width = region.imageExtent.x,
                    .height = region.imageExtent.y,
                    .depth = region.imageExtent.z
                }
            };
        }

        vkCmdCopyBufferToImage
        (
            commandBuffer,
            vulkanBuffer->GetVulkanBuffer(),
            vulkanTexture->GetVulkanImage(),
            ConvertTextureLayout( layout ),
            regionCount,
            regions.data()
        );
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

    void VulkanRHICommandBuffer::DrawIndexed( uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance ) const
    {
        vkCmdDrawIndexed( commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
    }

    void VulkanRHICommandBuffer::Draw( uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance ) const
    {
        vkCmdDraw( commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance );
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

        VkResult result = vkCreateShaderModule( device->GetVulkanDevice(), &createInfo, nullptr, &shaderModule );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_ERROR( "Failed to create shader module! {}", GetVKResultString( result ) );
            return;
        }
    }

    bool VulkanRHIShaderStage::IsValid() const
    {
        return shaderModule != VK_NULL_HANDLE;
    }

    const U8Char* VulkanRHIShaderStage::GetEntryPoint() const
    {
        return entryPoint;
    }

    struct SubpassDescriptionAttachments
    {
        TArray<VkAttachmentReference> input;
        TArray<VkAttachmentReference> color;
        VkAttachmentReference resolve;
    };

    VulkanRHIRenderPass::~VulkanRHIRenderPass()
    {
        vkDestroyRenderPass( device->GetVulkanDevice(), renderPass, nullptr );

        for ( auto& pair : framebufferAttachments )
        {
            vkDestroyFramebuffer( device->GetVulkanDevice(), pair.second, NULL );
        }
        framebufferAttachments.clear();
    }

    VulkanRHIRenderPass::VulkanRHIRenderPass( const RHIRenderPassCreateInfo& info, VulkanRHIDevice* device )
        : renderPass( VK_NULL_HANDLE )
        , device( device )
        , framebufferAttachments()
        , lastAttachment( VK_NULL_HANDLE )
        , renderArea()
        , clearColorValue()
        , clearDepthStencilValue()
    {
        uint32 subpassesSize = (uint32)info.subpasses.size();
        TArray<SubpassDescriptionAttachments> subpassAttachments(subpassesSize);
        TArray<VkSubpassDescription> subpasses(subpassesSize);
        for ( uint32 i = 0; i < subpassesSize; i++ )
        {
            const RHIRenderSubpassDescription& subpass = info.subpasses[ i ];
            SubpassDescriptionAttachments& attachments = subpassAttachments[ i ];

            uint32 inputAttachmentSize = (uint32)subpass.inputAttachmentReferences.size();
            // Use last input to store depth attachemnt
            attachments.input.resize( inputAttachmentSize + (subpass.usingDepth ? 1 : 0) );
            for ( uint32 j = 0; j < inputAttachmentSize; j++ )
            {
                attachments.input[ j ] = VkAttachmentReference
                {
                    .attachment = subpass.inputAttachmentReferences[ j ].atachmentIndex,
                    .layout = ConvertTextureLayout( subpass.inputAttachmentReferences[ j ].textureLayout ),
                };
            }

            uint32 colorAttachmentSize = (uint32)subpass.colorAttachmentReferences.size();
            attachments.color.resize( colorAttachmentSize );
            for ( uint32 j = 0; j < colorAttachmentSize; j++ )
            {
                attachments.color[ j ] = VkAttachmentReference
                {
                    .attachment = subpass.colorAttachmentReferences[ j ].atachmentIndex,
                    .layout = ConvertTextureLayout( subpass.colorAttachmentReferences[ j ].textureLayout ),
                };
            }

            if ( subpass.usingDepth )
            {
                VkAttachmentReference depthStencilAttachment
                {
                    .attachment = subpass.depthAttachment.atachmentIndex,
                    .layout = ConvertTextureLayout( subpass.depthAttachment.textureLayout ),
                };
                attachments.input[ inputAttachmentSize ] = depthStencilAttachment;
            }

            if ( subpass.hasResolve )
            {
                VkAttachmentReference attachmentResolve
                {
                    .attachment = subpass.resolveAttachmentReferences.atachmentIndex,
                    .layout = ConvertTextureLayout( subpass.resolveAttachmentReferences.textureLayout ),
                };
                attachments.resolve = attachmentResolve;
            }

            subpasses[ i ] = VkSubpassDescription
            {
                .flags = 0,
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                .inputAttachmentCount = inputAttachmentSize,
                .pInputAttachments = attachments.input.data(),
                .colorAttachmentCount = colorAttachmentSize,
                .pColorAttachments = attachments.color.data(),
                .pResolveAttachments = subpass.hasResolve ? &attachments.resolve : NULL,
                .pDepthStencilAttachment = subpass.usingDepth ? &attachments.input.back() : NULL,
                .preserveAttachmentCount = 0,
                .pPreserveAttachments = NULL
            };
        }

        uint32 attachmentSize = (uint32)info.attachments.size();
        TArray<VkAttachmentDescription> attachments( attachmentSize );
        TArray<VkSubpassDependency> dependencies( attachmentSize );
        for ( uint32 i = 0; i < attachmentSize; i++ )
        {
            const RHIAttachmentDescription& attachment = info.attachments[ i ];

            attachments[ i ] = VkAttachmentDescription
            {
                .flags = 0,
                .format = ConvertPixelFormat( attachment.format ),
                .samples = ConvertSampleCountBits( attachment.sampleCount ),
                .loadOp = ConvertLoadAttachmentOperation( attachment.colorLoadOperation ),
                .storeOp = ConvertStoreAttachmentOperation( attachment.colorStoreOperation ),
                .stencilLoadOp = ConvertLoadAttachmentOperation( attachment.stencilLoadOperation ),
                .stencilStoreOp = ConvertStoreAttachmentOperation( attachment.stencilStoreOperation ),
                .initialLayout = ConvertTextureLayout( attachment.initialLayout ),
                .finalLayout = ConvertTextureLayout( attachment.finalLayout )
            };

            if ( IsPixelFormatDepth( attachment.format ) )
            {
                dependencies[ i ] = VkSubpassDependency
                {
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,
                    .srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                    .srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    .dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
                    .dependencyFlags = 0
                };
            }
            else
            {
                dependencies[ i ] = VkSubpassDependency
                {
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,
                    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                    .dependencyFlags = 0
                };
            }
        }

        VkRenderPassCreateInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .attachmentCount = attachmentSize,
            .pAttachments = attachments.data(),
            .subpassCount = subpassesSize,
            .pSubpasses = subpasses.data(),
            .dependencyCount = attachmentSize,
            .pDependencies = dependencies.data()
        };

        VkResult result = vkCreateRenderPass( device->GetVulkanDevice(), &renderPassInfo, NULL, &renderPass );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create render pass {}", GetVKResultString( result ) );
        }
    }

    void VulkanRHIRenderPass::SetAttachments( uint32 attachmentCount, const RHITextureView** textureViews )
    {
        uint64 hash = 0;
        TArray<VkImageView> imageViews( attachmentCount );
        for ( uint32 i = 0; i < attachmentCount; i++ )
        {
            imageViews[ i ] = static_cast<const VulkanRHITextureView*>(textureViews[ i ])->GetVulkanImageView();
            HashCombine( &hash, imageViews[ i ] );
        }

        if ( framebufferAttachments.contains( hash ) == false )
        {
            VkFramebufferCreateInfo framebufferInfo
            {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .renderPass = renderPass,
                .attachmentCount = attachmentCount,
                .pAttachments = imageViews.data(),
                .width = textureViews[ 0 ]->GetTexture()->GetExtents().x,
                .height = textureViews[ 0 ]->GetTexture()->GetExtents().y,
                .layers = 1
            };

            VkFramebuffer framebuffer;
            VkResult result = vkCreateFramebuffer( device->GetVulkanDevice(), &framebufferInfo, nullptr, &framebuffer );
            if ( result != VK_SUCCESS )
            {
                EE_LOG_CRITICAL( "Failed to create frame buffer! {}", GetVKResultString( result ) );
                return;
            }

            framebufferAttachments.emplace( hash, framebuffer );
            lastAttachment = framebuffer;
        }
        else
        {
            lastAttachment = framebufferAttachments[ hash ];
        }
    }

    void VulkanRHIRenderPass::SetDrawArea( const IntBox2& extent )
    {
        renderArea =
        {
            .offset = { extent.minX, extent.minY },
            .extent = { (uint32)extent.maxX, (uint32)extent.maxY }
        };
    }

    void VulkanRHIRenderPass::SetClearValues( const Vector4f& color, const float& depth, const uint32& stencil )
    {
        clearColorValue.color = { color.r, color.g, color.b, color.a };
        clearDepthStencilValue.depthStencil.depth = depth;
        clearDepthStencilValue.depthStencil.stencil = stencil;
    }

    void VulkanRHIRenderPass::BeginRenderPass( const RHICommandBuffer* cmd )
    {
        VkRenderPassBeginInfo info
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = NULL,
            .renderPass = renderPass,
            .framebuffer = lastAttachment,
            .renderArea = renderArea,
            .clearValueCount = EE_ARRAYSIZE( clearValues ),
            .pClearValues = clearValues
        };

        const VulkanRHICommandBuffer* commandBuffer = static_cast<const VulkanRHICommandBuffer*>(cmd);
        vkCmdBeginRenderPass( commandBuffer->GetVulkanCommandBuffer(), &info, VK_SUBPASS_CONTENTS_INLINE );
    }

    void VulkanRHIRenderPass::EndRenderPass( const RHICommandBuffer* cmd )
    {
        const VulkanRHICommandBuffer* commandBuffer = static_cast<const VulkanRHICommandBuffer*>( cmd );
        vkCmdEndRenderPass( commandBuffer->GetVulkanCommandBuffer() );
    }

    bool VulkanRHIRenderPass::IsValid() const
    {
        return renderPass != VK_NULL_HANDLE;
    }

    VulkanRHIBindLayout::~VulkanRHIBindLayout()
    {
        vkDestroyDescriptorSetLayout( device->GetVulkanDevice(), descriptorSetLayout, NULL );
    }

    VulkanRHIBindLayout::VulkanRHIBindLayout( const RHIBindGroupCreateInfo& info, VulkanRHIDevice* device )
        : device( device )
        , descriptorSetLayout( VK_NULL_HANDLE )
    {
        uint32 bindingsCount = (uint32)info.bindings.size();
        TArray<VkDescriptorSetLayoutBinding> bindings( bindingsCount );

        for ( uint32 j = 0; j < bindingsCount; j++ )
        {
            const RHIResourceBinding& binding = info.bindings[ j ];

            bindings[ j ] =
                VkDescriptorSetLayoutBinding
                {
                    .binding = binding.index,
                    .descriptorType = ConvertDescriptorType( binding.type ),
                    .descriptorCount = 1,
                    .stageFlags = ConvertShaderStageFlags( binding.shaderVisibility ),
                    .pImmutableSamplers = NULL,
                };
        }

        VkDescriptorSetLayoutCreateInfo descriptorInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .bindingCount = bindingsCount,
            .pBindings = bindings.data()
        };

        VkResult descriptorResult = vkCreateDescriptorSetLayout( device->GetVulkanDevice(), &descriptorInfo, NULL, &descriptorSetLayout );
        if ( descriptorResult != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create descriptor set layout {}", GetVKResultString( descriptorResult ) );
        }
    }

    bool VulkanRHIBindLayout::IsValid() const
    {
        return descriptorSetLayout != VK_NULL_HANDLE;
    }

    VulkanRHIBindGroup::~VulkanRHIBindGroup()
    {
        // vkFreeDescriptorSets( device_->GetVulkanDevice(), pool_, 1, &descriptorSet );
        vkDestroyDescriptorPool( device_->GetVulkanDevice(), pool_, NULL );
    }

    const void VulkanRHIBindGroup::CreateDescriptorPool( const RHIBindGroupCreateInfo& info )
    {
        const uint32 bindingCount = (uint32)info.bindings.size();

        TArray<VkDescriptorPoolSize> poolSizes( bindingCount );
        for ( uint32 i = 0; i < bindingCount; i++ )
        {
            const RHIResourceBinding& binding = info.bindings[ i ];

            poolSizes[ i ].type = ConvertDescriptorType( binding.type );
            poolSizes[ i ].descriptorCount = 1;
        }

        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .maxSets = 1,
            .poolSizeCount = bindingCount,
            .pPoolSizes = poolSizes.data()
        };

        VkResult result = vkCreateDescriptorPool( device_->GetVulkanDevice(), &poolInfo, NULL, &pool_ );
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create descriptor set pool {}", GetVKResultString( result ) );
        }
    }

    VulkanRHIBindGroup::VulkanRHIBindGroup( const RHIBindGroupCreateInfo& info, VulkanRHIDevice* device )
        : bindLayout_( info, device )
        , device_( device )
        , descriptorSet_( VK_NULL_HANDLE )
        , pool_( VK_NULL_HANDLE )
    {
        CreateDescriptorPool( info );

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = NULL,
            .descriptorPool = pool_,
            .descriptorSetCount = 1,
            .pSetLayouts = &bindLayout_.GetVulkanDescriptorSetLayout()
        };

        VkResult allocateResult = vkAllocateDescriptorSets( device->GetVulkanDevice(), &allocInfo, &descriptorSet_ );
        if ( allocateResult != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to allocate descriptor set {}", GetVKResultString( allocateResult ) );
        }

        const uint32 bindingCount = (uint32)info.bindings.size();

        TArray<VkWriteDescriptorSet> descriptorWrites( bindingCount );
        TArray<VkDescriptorImageInfo> imageInfos;
        TArray<VkDescriptorBufferInfo> bufferInfos;

        uint32 imageInfosNum = 0;
        uint32 bufferInfosNum = 0;
        for ( uint32 i = 0; i < bindingCount; i++ )
        {
            const auto& binding = info.bindings[ i ];

            switch ( binding.type )
            {
            case BindingType_Uniform:
            case BindingType_Storage:
            case BindingType_UniformDynamic:
            case BindingType_StorageDynamic:
            {
                bufferInfosNum++;
            } break;
            case BindingType_Sampler:
            case BindingType_TextureView:
            case BindingType_TextureStorage:
            {
                imageInfosNum++;
            } break;
            default:
                break;
            }
        }
        imageInfos.reserve( imageInfosNum );
        bufferInfos.reserve( bufferInfosNum );

        for ( uint32 i = 0; i < bindingCount; i++ )
        {
            const RHIResourceBinding& binding = info.bindings[ i ];

            VkWriteDescriptorSet writeDescriptorSet
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = NULL,
                .dstSet = descriptorSet_,
                .dstBinding = binding.index,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = ConvertDescriptorType( binding.type ),
                .pImageInfo = NULL,
                .pBufferInfo = NULL,
                .pTexelBufferView = NULL
            };

            switch ( binding.type )
            {
            case BindingType_Uniform:
            case BindingType_Storage:
            case BindingType_UniformDynamic:
            case BindingType_StorageDynamic:
            {
                const VulkanRHIBuffer* vulkanBuffer = static_cast<const VulkanRHIBuffer*>(binding.resource);

                if ( ( binding.type == BindingType_Uniform || binding.type == BindingType_UniformDynamic) 
                    && ( binding.bufferRange > device->GetLimits().maxUniformBufferRange ) )
                {
                    EE_LOG_WARN( "Uniform buffer binding has a range ({}), which is higher than the device limits ({}).",
                        binding.bufferRange, device->GetLimits().maxUniformBufferRange );
                }
                VkDescriptorBufferInfo bufferInfo
                {
                    .buffer = vulkanBuffer->GetVulkanBuffer(),
                    .offset = vulkanBuffer->GetOffset(),
                    .range = binding.bufferRange,
                };
                bufferInfos.emplace_back( bufferInfo );

                writeDescriptorSet.pBufferInfo = &bufferInfos.back();
            } break;
            case BindingType_Sampler:
            {
                const VulkanRHISampler* sampler = static_cast<const VulkanRHISampler*>( binding.resource );
                VkDescriptorImageInfo imageInfo
                {
                    .sampler = sampler->GetVulkanSampler(),
                    .imageView = VK_NULL_HANDLE,
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                };
                imageInfos.emplace_back( imageInfo );

                writeDescriptorSet.pImageInfo = &imageInfos.back();
            } break;
            case BindingType_TextureView:
            {
                const VulkanRHITextureView* textureView = static_cast<const VulkanRHITextureView*>( binding.resource );
                VkDescriptorImageInfo imageInfo
                {
                    .sampler = VK_NULL_HANDLE,
                    .imageView = textureView->GetVulkanImageView(),
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                };
                imageInfos.emplace_back( imageInfo );

                writeDescriptorSet.pImageInfo = &imageInfos.back();
            } break;
            default:
                break;
            }

            descriptorWrites[ i ] = writeDescriptorSet;
        }

        vkUpdateDescriptorSets( device->GetVulkanDevice(), bindingCount, descriptorWrites.data(), 0, NULL );
    }

    bool VulkanRHIBindGroup::IsValid() const
    {
        return descriptorSet_ != VK_NULL_HANDLE;
    }

    const RHIBindLayout& VulkanRHIBindGroup::GetBindLayout() const
    {
        return bindLayout_;
    }

    VulkanRHIGraphicsPipeline::~VulkanRHIGraphicsPipeline()
    {
        vkDestroyPipelineLayout( device->GetVulkanDevice(), pipelineLayout, NULL );
        vkDestroyPipeline( device->GetVulkanDevice(), pipeline, NULL );
    }

    VulkanRHIGraphicsPipeline::VulkanRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info, VulkanRHIDevice* device )
        : descriptorSetLayouts()
        , pipelineLayout( VK_NULL_HANDLE )
        , pipeline( VK_NULL_HANDLE )
        , device( device )
    {
        TArray<VkPipelineShaderStageCreateInfo> stages;
        const auto AddStage = [&stages]( const RHIShaderStageAttachment& shaderAttachment )
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

        uint32 bindingsSize = (uint32)info.vertexState.bufferLayouts.size();
        TArray<VkVertexInputAttributeDescription> attributes;
        TArray<VkVertexInputBindingDescription> bindings;
        bindings.resize( bindingsSize );

        for ( uint32 i = 0; i < bindingsSize; ++i )
        {
            const auto& binding = info.vertexState.bufferLayouts[ i ];
            bindings[ i ].binding = i;
            bindings[ i ].inputRate = ConvertVertexStepMode( binding.stepMode );
            bindings[ i ].stride = binding.stride;

            for ( size_t j = 0; j < binding.attributes.size(); ++j )
            {
                VkVertexInputAttributeDescription desc = {};
                desc.binding = i;
                desc.location = binding.attributes[ j ].semanticIndex;
                desc.offset = binding.attributes[ j ].offset;
                desc.format = ConvertVertexFormat( binding.attributes[ j ].format );
                attributes.emplace_back( desc );
            }
        }
        uint32 attributesSize = (uint32)attributes.size();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .vertexBindingDescriptionCount = bindingsSize,
            .pVertexBindingDescriptions = bindings.data(),
            .vertexAttributeDescriptionCount = attributesSize,
            .pVertexAttributeDescriptions = attributes.data(),
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
            .rasterizationSamples = ConvertSampleCountBits( info.sampleCount ),
            .sampleShadingEnable = info.sampleCount != SampleCount_1_Bit,
            .minSampleShading = .1F,            // Min fraction for sample shading; closer to one is smoot
            .pSampleMask = NULL,                // Optional
            .alphaToCoverageEnable = VK_FALSE,  // Optional
            .alphaToOneEnable = VK_FALSE,       // Optional
        }; 
        
        VkPipelineDepthStencilStateCreateInfo depStencilInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .depthTestEnable = info.depthStencilState.depthEnabled,
            .depthWriteEnable = info.depthStencilState.depthWriteEnabled,
            .depthCompareOp = ConvertCompareOperation( info.depthStencilState.depthCompareOperation ),
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = info.depthStencilState.stencilEnabled,
            .front =
            {
                .failOp = ConvertStencilOperation( info.depthStencilState.stencilFront.failOperation ),
                .passOp = ConvertStencilOperation( info.depthStencilState.stencilFront.passOperation ),
                .depthFailOp = ConvertStencilOperation( info.depthStencilState.stencilFront.depthFailOperation ),
                .compareOp = ConvertCompareOperation( info.depthStencilState.stencilFront.compareOperation ),
                .compareMask = info.depthStencilState.stencilFront.compareMask,
                .writeMask = info.depthStencilState.stencilFront.writeMask,
                .reference = info.depthStencilState.stencilFront.reference
            },
            .back =
            {
                .failOp = ConvertStencilOperation( info.depthStencilState.stencilBack.failOperation ),
                .passOp = ConvertStencilOperation( info.depthStencilState.stencilBack.passOperation ),
                .depthFailOp = ConvertStencilOperation( info.depthStencilState.stencilBack.depthFailOperation ),
                .compareOp = ConvertCompareOperation( info.depthStencilState.stencilBack.compareOperation ),
                .compareMask = info.depthStencilState.stencilBack.compareMask,
                .writeMask = info.depthStencilState.stencilBack.writeMask,
                .reference = info.depthStencilState.stencilBack.reference,
            },
            .minDepthBounds = 0.0F,
            .maxDepthBounds = 1.0F,
        };

        const uint32 setLayoutsCount = (uint32)info.bindLayouts.size();
        descriptorSetLayouts.resize( setLayoutsCount );

        for ( uint32 i = 0; i < setLayoutsCount; i++ )
        {
            const VulkanRHIBindLayout* vulkanBindLayout = static_cast<const VulkanRHIBindLayout*>(info.bindLayouts[ i ]);
            descriptorSetLayouts[ i ] = vulkanBindLayout->GetVulkanDescriptorSetLayout();
        }

        const uint32 colorAttachmentCount = (uint32)info.colorAttachments.size();
        TArray<VkPipelineColorBlendAttachmentState> colorAttachments( colorAttachmentCount );

        TArray<VkFormat> pixelFormats( colorAttachmentCount );
        for ( uint32 i = 0; i < colorAttachmentCount; i++ )
        {
            auto format = info.colorAttachments[ i ].format;
            pixelFormats[ i ] = ConvertPixelFormat( info.colorAttachments[ i ].format );
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
            .setLayoutCount = setLayoutsCount,
            .pSetLayouts = descriptorSetLayouts.data(),
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = NULL
        }; 
        
        VkResult layoutResult = vkCreatePipelineLayout( device->GetVulkanDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout );
        if ( layoutResult != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create pipeline layout {}", GetVKResultString( layoutResult ) );
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

        const VulkanRHIRenderPass* vulkanRenderPass = static_cast<const VulkanRHIRenderPass*>( info.renderpass );

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
            .pDepthStencilState = &depStencilInfo,
            .pColorBlendState = &colorInfo,
            .pDynamicState = &dynamicState,
            .layout = pipelineLayout,
            .renderPass = vulkanRenderPass->GetVulkanRenderPass(),
            .subpass = info.subpassIndex,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        VkResult result = vkCreateGraphicsPipelines( device->GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &pipeline);
        if ( result != VK_SUCCESS )
        {
            EE_LOG_CRITICAL( "Failed to create graphics pipeline {}", GetVKResultString( result ) );
        }
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

        EE_LOG_CRITICAL( "Failed to find suitable memory type!" );
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

        if ( SDL_Vulkan_LoadLibrary( nullptr ) == false )
        {
            EE_LOG_CRITICAL( "Failed to load SDL Vulkan Library! {}", SDL_GetError() );
        }

        uint32 layerCount;
        vkEnumerateInstanceLayerProperties( &layerCount, nullptr );
        TArray<VkLayerProperties> availableLayers( layerCount );
        vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

        TArray<const U8Char*> layers;
#if defined(EMPTYENGINE_CORE_LOG) && defined(EE_CORE_VULKAN_VALIDATION_LAYER)
        for ( uint32 i = 0; i < layerCount; i++ )
        {
            if ( std::strcmp( availableLayers[ i ].layerName, "VK_LAYER_KHRONOS_validation" ) == 0 )
            {
                layers.emplace_back( availableLayers[ i ].layerName );
            }
        }
#endif

        uint32 extensionCount;
        const U8Char* const* extensionNames = NULL;
        GetVulkanInstanceExtensions( &extensionCount, extensionNames );

        TArray<const U8Char*> extensions;
        for ( uint32 i = 0; i < extensionCount; i++ )
        {
            extensions.emplace_back( extensionNames[ i ] );
        }
#ifdef EMPTYENGINE_CORE_LOG
        extensions.emplace_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
#endif

        VkValidationFeatureEnableEXT validationFeatureEnables[] =
        {
            VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
        };

        VkValidationFeaturesEXT validationFeatures
        {
            .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
            .pNext = NULL,
            .enabledValidationFeatureCount = EE_ARRAYSIZE( validationFeatureEnables ),
            .pEnabledValidationFeatures = validationFeatureEnables,
            .disabledValidationFeatureCount = 0,
            .pDisabledValidationFeatures = NULL
        };

        VkInstanceCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = &validationFeatures,
            .flags = 0,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = (uint32)layers.size(),
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = (uint32)extensions.size(),
            .ppEnabledExtensionNames = extensions.data()
        };

        GVulkanInstance = new VulkanRHIInstance( createInfo );

#if defined(EMPTYENGINE_CORE_LOG) && defined(EE_CORE_VULKAN_VALIDATION_LAYER)
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
        vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( GVulkanInstance->GetVulkanInstance(), "vkCreateDebugUtilsMessengerEXT");
        if ( vkCreateDebugUtilsMessenger == VK_NULL_HANDLE )
        {
            EE_LOG_CRITICAL( "Failed to find function vkCreateDebugUtilsMessengerEXT" );
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
            EE_LOG_CRITICAL( "Failed to create vulkan messenger {}", GetVKResultString( result ) );
        }
#endif
    }

    void VulkanRHI::SetName( RHIResource* pResource, const U8Char* name ) { }

    void VulkanRHI::WaitForDevice() const { }

    const RHIDevice* VulkanRHI::GetRHIDevice() const
    {
        return GVulkanDevice;
    }

    const U8String& VulkanRHI::GetName() const
    {
        static U8String const deviceName = "Vulkan " 
            + Text::FormatUnit( VK_API_VERSION_MAJOR( VK_HEADER_VERSION_COMPLETE ), 0 ) + '.'
            + Text::FormatUnit( VK_API_VERSION_MINOR( VK_HEADER_VERSION_COMPLETE ), 0 ) + '.' 
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
        return new VulkanRHIBuffer( info, GVulkanDevice );
    }

    RHITexture* VulkanRHI::CreateRHITexture( const RHITextureCreateInfo& info ) const
    {
        return new VulkanRHITexture( info, GVulkanDevice );
    }

    RHITextureView* VulkanRHI::CreateRHITextureView( const RHITextureViewCreateInfo& info ) const
    {
        return new VulkanRHITextureView( info, GVulkanDevice );
    }
    
    RHISampler* VulkanRHI::CreateRHISampler( const RHISamplerCreateInfo& info ) const
    {
        return new VulkanRHISampler( info, GVulkanDevice );
    }
    
    RHIShaderStage* VulkanRHI::CreateRHIShaderStage( const RHIShaderStageCreateInfo& info ) const
    {
        return new VulkanRHIShaderStage( info, GVulkanDevice );
    }

    RHIBindGroup* VulkanRHI::CreateRHIBindGroup( const RHIBindGroupCreateInfo& info ) const
    {
        return new VulkanRHIBindGroup( info, GVulkanDevice );
    }

    RHIRenderPass* VulkanRHI::CreateRHIRenderPass( const RHIRenderPassCreateInfo& info ) const
    {
        return new VulkanRHIRenderPass( info, GVulkanDevice );
    }
    
    RHIGraphicsPipeline* VulkanRHI::CreateRHIGraphicsPipeline( const RHIGraphicsPipelineCreateInfo& info ) const
    {
        return new VulkanRHIGraphicsPipeline( info, GVulkanDevice );
    }

    void* GetVulkanRHIInstance()
    {
        return GVulkanInstance;
    }
}