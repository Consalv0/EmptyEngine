#pragma once

namespace EE
{
    class RHIObject
    {
    public:
        virtual bool IsValid() const = 0;
    };

    class RHIResource : public RHIObject
    {
    public:
        ERenderingResourceType type = RenderingResourceType_Unknown;
        FORCEINLINE bool IsTexture()   const { return type == RenderingResourceType_Texture; }
        FORCEINLINE bool IsBuffer()    const { return type == RenderingResourceType_Buffer; }
        FORCEINLINE bool IsSwapChain() const { return type == RenderingResourceType_Swapchain; }
        FORCEINLINE bool IsSurface()   const { return type == RenderingResourceType_Surface; }
        FORCEINLINE bool IsShader()    const { return type == RenderingResourceType_Shader; }
    };

    class RHISurface : public RHIResource
    {
    };

    struct RHISwapChainCreateDescription
    {
        uint32 width = 0;
        uint32 height = 0;
        uint32 bufferCount = 2;
        EPixelFormat format = PixelFormat_Unknown;
        EColorSpace colorSpace = ColorSpace_Unknown;
        bool fullscreen = false;
        bool vsync = true;
        Vector4 clearcolor = { 0,0,0,1 };
    };

    class RHISwapChain : public RHIResource
    {
    };

    class RHIInstance : public RHIObject
    {
    };

    class RHIPresentContext : public RHIObject
    {
    };

    class RHIDevice : public RHIObject
    {
    };

    struct RHIBufferCreateDescription
    {
        uint64 size = 0;
        EBufferUsageFlags usages = BufferUsage_None;
        ESharingMode sharing = SharingMode_Default;
    };

    class RHIShaderStage : public RHIResource
    {
        EShaderStage stage = ShaderStage_Unknown;
    };

    struct RHICommandBufferCreateDescription
    {
    };

    class RHICommandBuffer : public RHIResource
    {
    };

    class RHIBuffer : public RHIResource
    {
    };

    struct RHITextureCreateDescription
    {
        ETextureType type = TextureType_Texture2D;
        uint32 width = 0;
        uint32 height = 0;
        uint32 depth = 0;
        uint32 arraySize = 1;
        uint32 mipLevels = 1;
        EPixelFormat format = PixelFormat_Unknown;
        EColorSpace colorSpace = ColorSpace_Unknown;
        ETilingMode tiling = TilingMode_Default;
        uint32 sampleCount = 1;
        EUsageModeFlags usage = UsageMode_Color;
        EUsageModeFlags viewUsage = UsageMode_Color;
        ESharingMode sharing = SharingMode_Default;
        uint32 bindFlags = 0;
        uint32 accessFlags = 0;
        uint32 miscFlags = 0;
        Vector4 clear = {};
        EImageLayout layout = ImageLayout_ShaderResource;
    };

    typedef class RHITexture* RHITextureRef;

    class RHITexture : public RHIResource
    {
    };

    struct RHISamplerCreateDescription
    {
        EFilterMode filter = FilterMode_MinMagNearest;
        ESamplerAddressMode addressU = SamplerAdressMode_Clamp;
        ESamplerAddressMode addressV = SamplerAdressMode_Clamp;
        ESamplerAddressMode addressW = SamplerAdressMode_Clamp;
        float mipLODBias = 0.0f;
        uint32 maxAnisotropy = 0;
        EComparisonFunction comparison = ComparisonFuntion_Always;
        ESamplerBorder border = SamplerBorder_BlackTransparent;
        float minLOD = 0.0f;
        float maxLOD = FLT_MAX;
    };

    class RHISampler : public RHIObject
    {
    };

    struct PipelineStateDescription
    {
        const RHIShaderStage* vertex = nullptr;
        const RHIShaderStage* pixel = nullptr;
        const RHIShaderStage* hull = nullptr;
        const RHIShaderStage* ds = nullptr;
        const RHIShaderStage* geometry = nullptr;
        const RHIShaderStage* ms = nullptr;
        const RHIShaderStage* as = nullptr;

        uint32 sampleMask = 0xFFFFFFFF;
    };
}