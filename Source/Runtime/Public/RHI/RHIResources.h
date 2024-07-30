#pragma once

namespace EE
{
    class RHIObject
    {
    public:
        EE_CLASSNOCOPY( RHIObject )

    protected:
        RHIObject() {}
        ~RHIObject() {}

    public:
        virtual bool IsValid() const = 0;
    };

    class RHIResource : public RHIObject
    {
    protected:
        RHIResource() {}
        ~RHIResource() {}

    public:
        // FORCEINLINE bool IsTexture()   const { return type == RenderingResourceType_Texture; }
        // FORCEINLINE bool IsBuffer()    const { return type == RenderingResourceType_Buffer; }
        // FORCEINLINE bool IsSwapChain() const { return type == RenderingResourceType_Swapchain; }
        // FORCEINLINE bool IsSurface()   const { return type == RenderingResourceType_Surface; }
        // FORCEINLINE bool IsShader()    const { return type == RenderingResourceType_Shader; }
    };

    class RHISurface : public RHIResource
    {
    protected:
        RHISurface() {}
    public:
        virtual ~RHISurface() {};
    };

    class RHITexture : public RHIResource
    {
    protected:
        RHITexture() {}
    public:
        virtual ~RHITexture() {};

        virtual const UIntVector3& GetExtent() const = 0;
    };

    class RHIFence : public RHIResource
    {
    protected:
        RHIFence() {}
    public:
        virtual ~RHIFence() {};

        virtual bool IsSignaled() const = 0;
        virtual void Reset() const = 0;
        virtual void Wait( uint64 timeout ) const = 0;
    };

    class RHISemaphore : public RHIResource
    {
    protected:
        RHISemaphore() {}
    public:
        virtual ~RHISemaphore() {};
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
    protected:
        RHISwapChain() {}
    public:
        virtual ~RHISwapChain() {};
    };

    class RHIInstance : public RHIObject
    {
    protected:
        RHIInstance() {}
    public:
        virtual ~RHIInstance() {};
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
    protected:
        RHICommandBuffer() {}
    public:
        virtual ~RHICommandBuffer() {};

        virtual void Begin() const = 0;

        virtual void End() const = 0;

        virtual void ClearColor( Vector3f color, const RHITexture* texture ) const = 0;
    };

    class RHIPresentContext : public RHIObject
    {
    protected:
        RHIPresentContext() {}
    public:
        virtual ~RHIPresentContext() {};

        virtual const RHITexture* GetBackbuffer( uint32 index ) const = 0;

        virtual uint32 AquireBackbuffer( uint64 timeout ) const = 0;

        virtual void Present( uint32 imageIndex ) const = 0;

        virtual void SubmitCommandBuffer( uint32 imageIndex ) const = 0;

        virtual const RHICommandBuffer* GetCommandBuffer( uint32 imageIndex ) const = 0;

        virtual const RHIFence* GetFence( uint32 imageIndex ) const = 0;
    };

    struct RHIQueueSubmitInfo
    {
    public:
        std::vector<const RHISemaphore*> waitSemaphores;
        std::vector<const RHISemaphore*> signalSemaphores;
        EPipelineStage stageFlags;

        const RHIFence* signalFence;
    };

    class RHIQueue : public RHIResource
    {
    protected:
        RHIQueue() {}
    public:
        virtual ~RHIQueue() {};

        virtual void SubmitCommandBuffer( const RHICommandBuffer* commandBuffer, const RHIQueueSubmitInfo& info ) = 0;
    };

    class RHIDevice : public RHIObject
    {
    protected:
        RHIDevice() {}
    public:
        virtual ~RHIDevice() {};

        virtual RHIQueue* GetGraphicsQueue() const = 0;

        virtual RHIQueue* GetPresentQueue() const = 0;
    };

    class RHIBuffer : public RHIResource
    {
    protected:
        RHIBuffer() {}
    public:
        virtual ~RHIBuffer() {};
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

    struct RHISamplerCreateDescription
    {
        EFilterMode filter = FilterMode_MinMagNearest;
        ESamplerAddressMode addressU = SamplerAdressMode_Clamp;
        ESamplerAddressMode addressV = SamplerAdressMode_Clamp;
        ESamplerAddressMode addressW = SamplerAdressMode_Clamp;
        float mipLODBias = 0.0F;
        uint32 maxAnisotropy = 0;
        EComparisonFunction comparison = ComparisonFuntion_Always;
        ESamplerBorder border = SamplerBorder_BlackTransparent;
        float minLOD = 0.0F;
        float maxLOD = FLT_MAX;
    };

    class RHISampler : public RHIObject
    {
    protected:
        RHISampler() {}
    public:
        virtual ~RHISampler() {};
    };

    struct RHIPipelineStateDescription
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

    class RHIRasterPipeline : public RHIObject
    {
    protected:
        RHIRasterPipeline() {}
    public:
        virtual ~RHIRasterPipeline() {};
    };
}