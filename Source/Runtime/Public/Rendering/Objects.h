#pragma once

namespace EE
{
    struct RenderObject
    {
        std::shared_ptr<void> internalState;
        FORCEINLINE bool IsValid() const { return internalState.get() != nullptr; }
    };

    struct RenderResource : public RenderObject
    {
        ERenderingResourceType type = RenderingResourceType_Unknown;
        FORCEINLINE bool IsTexture() const { return type == RenderingResourceType_Texture; }
        FORCEINLINE bool IsBuffer() const { return type == RenderingResourceType_Buffer; }
        FORCEINLINE bool IsSwapChain() const { return type == RenderingResourceType_Swapchain; }
        FORCEINLINE bool IsSurface() const { return type == RenderingResourceType_Surface; }
    };

    struct SurfaceDescription
    {
        class Window* window;
    };

    struct Surface : public RenderResource
    {
        SurfaceDescription description;
        FORCEINLINE const SurfaceDescription& GetDescription() const { return description; }
    };

    struct SwapChainDescription
    {
        uint32 width = 0;
        uint32 height = 0;
        uint32 buffercount = 2;
        EPixelFormat format = PixelFormat_Unknown;
        EColorSpace colorSpace = ColorSpace_Unknown;
        bool fullscreen = false;
        bool vsync = true;
        Vector4 clearcolor = { 0,0,0,1 };
    };

    struct SwapChain : public RenderResource
    {
        SwapChainDescription description;
        FORCEINLINE const SwapChainDescription& GetDescription() const { return description; }
    };

    struct RenderInstance : RenderObject
    {
    };

    struct PresentContext : RenderObject
    {
        Surface surface;
        SwapChain swapChain;
    };

    struct RenderDevice : RenderObject
    {
        PresentContext context;
    };

    struct BufferDescription
    {
        uint64 size = 0;
        EBufferUsageFlags usages;
        ESharingMode sharing;
    };

    struct ShaderStage : public RenderResource
    {
        EShaderStage stage = ShaderStage_Unknown;
    };

    struct Buffer : public RenderResource
    {
        BufferDescription description;
        FORCEINLINE const BufferDescription& GetDescription() const { return description; }
    };

    struct TextureDescription
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
        EUsageMode usage = UsageMode_Color;
        ESharingMode sharing = SharingMode_Default;
        uint32 bindFlags = 0;
        uint32 accessFlags = 0;
        uint32 miscFlags = 0;
        Vector4 clear = {};
        EImageLayout layout = ImageLayout_ShaderResource;
    };

    struct Texture : public RenderResource
    {
        TextureDescription description;
        FORCEINLINE const TextureDescription& GetDescription() const { return description; }
    };

    struct SamplerDescription
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

    struct Sampler : public RenderObject
    {
        SamplerDescription description;
        FORCEINLINE const SamplerDescription& GetDescription() const { return description; }
    };

    struct PipelineStateDescription
    {
        const ShaderStage* vertex = nullptr;
        const ShaderStage* pixel = nullptr;
        const ShaderStage* hull = nullptr;
        const ShaderStage* ds = nullptr;
        const ShaderStage* geometry = nullptr;
        const ShaderStage* ms = nullptr;
        const ShaderStage* as = nullptr;

        uint32 sampleMask = 0xFFFFFFFF;
    };
}