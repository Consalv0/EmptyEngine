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

    struct RHITextureCreateInfo
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
        EUsageModeFlags usage = UsageMode_Color_Bit;
        EUsageModeFlags viewUsage = UsageMode_Color_Bit;
        ESharingMode sharing = SharingMode_Default;
        uint32 bindFlags = 0;
        uint32 accessFlags = 0;
        uint32 miscFlags = 0;
        Vector4 clear = {};
        EImageLayout layout = ImageLayout_ShaderResource;
    };

    class RHITexture : public RHIResource
    {
    protected:
        RHITexture() {}

    public:
        virtual ~RHITexture() {};

        virtual const UIntVector3& GetExtent() const = 0;

        virtual const EPixelFormat& GetFormat() const = 0;
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

    struct RHISwapChainCreateInfo
    {
        Window* window = NULL;
        uint32 width = 0;
        uint32 height = 0;
        uint32 bufferCount = 2;
        EPixelFormat format = PixelFormat_Unknown;
        EColorSpace colorSpace = ColorSpace_Unknown;
        EPresentMode presentMode = PresentMode_VSync;
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

    struct RHIShaderStageCreateInfo
    {
        EShaderStage stage;
        EShaderFormat format;
        size_t codeLength;
        const void* code;
        const NChar* entryPoint;
    };

    class RHIShaderStage : public RHIResource
    {
        EShaderStage stage = ShaderStage_Unknown;

    protected:
        RHIShaderStage( EShaderStage stage ) : stage( stage ) { }

    public:
        virtual ~RHIShaderStage() {};

        FORCEINLINE const EShaderStage& GetStage() const { return stage; }
        
        virtual const NChar* GetEntryPoint() const = 0;
    };

    struct RHICommandBufferCreateInfo
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

        virtual void TransitionTexture( const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer, const ETextureLayout from, const ETextureLayout to ) const = 0;

        virtual void ClearColor( Vector3f color, const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer ) const = 0;
    };

    class RHIPresentContext : public RHIObject
    {
    protected:
        uint32 currentFrameIndex;

    protected:
        RHIPresentContext() : currentFrameIndex( 0 ) {}

    public:
        virtual ~RHIPresentContext() {};

        virtual const RHITexture* GetBackbuffer() const = 0;

        virtual void SubmitRenderCommandBuffer( EPipelineStage stage ) const = 0;

        virtual const RHICommandBuffer* GetRenderCommandBuffer() const = 0;

        virtual const RHIFence* GetRenderFence() const = 0;

        virtual void AquireBackbuffer( uint64 timeout ) = 0;

        FORCEINLINE const uint32& CurrentFrameIndex() const { return currentFrameIndex; }

        virtual const EPixelFormat& GetFormat() const = 0;

        virtual void Present() = 0;
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

    struct RHIBufferCreateInfo
    {
        uint64 size = 0;
        EBufferUsageFlags usages = BufferUsage_None;
        ESharingMode sharing = SharingMode_Default;
    };

    class RHIBuffer : public RHIResource
    {
    protected:
        RHIBuffer() {}
    public:
        virtual ~RHIBuffer() {};
    };

    struct RHISamplerCreateInfo
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

    struct RHIShaderStageAttachment
    {
        const RHIShaderStage* shaderStage;
    };

    struct RHIPrimitiveState
    {
        ETopologyMode topologyMode = TopologyMode_TriangleList;
        ERasterMode rasterMode = RasterMode_Solid;
        ECullModeFlags cullMode = CullMode_Back_Bit;
        EFaceWinding frontFace = FaceWinding_CounterClokwise;
        // Clip or clamp depth
        bool depthClip = false;
    };

    struct RHIStencilFaceState
    {
        EStencilFunction compareFunction = StencilFunction_Always;
        EStencilOperation failOperation = StencilOperation_Keep;
        EStencilOperation depthFailOperation = StencilOperation_Keep;
        EStencilOperation passOperation = StencilOperation_Keep;
    };

    struct RHIDepthStencilState
    {
        bool depthEnabled = false;
        bool stencilEnabled = false;
        EPixelFormat format = PixelFormat_MAX;
        EComparisonFunction depthCompareFunction = ComparisonFuntion_Always;
        int32 depthBias = 0;
        float depthBiasSlopeScale = 0.0F;
        float depthBiasClamp = 0.0F;
        RHIStencilFaceState stencilFront = RHIStencilFaceState();
        RHIStencilFaceState stencilBack = RHIStencilFaceState();
        uint8 stencilReadMask = 0u;
        uint8 stencilWriteMask = 0u;
    };

    struct RHIBlendComponent
    {
        EBlendOperation operation = BlendOperation_Add;
        EBlendFactor srcFactor = BlendFactor_One;
        EBlendFactor dstFactor = BlendFactor_Zero;
    };

    struct RHIColorAttachmentState
    {
        EPixelFormat format = PixelFormat_MAX;
        EColorComponentFlags writeFlags = ColorComponent_RGBA;
        bool blendEnabled = false;
        RHIBlendComponent colorBlend = RHIBlendComponent();
        RHIBlendComponent alphaBlend = RHIBlendComponent();
    };

    class RHIGraphicsPipelineCreateInfo
    {
    public:
        RHIShaderStageAttachment vertexShader;
        RHIShaderStageAttachment fragmentShader;
        RHIShaderStageAttachment geometryShader;
        RHIShaderStageAttachment domainShader;
        RHIShaderStageAttachment hullShader;

        TArray<RHIColorAttachmentState> colorAttachments;
        RHIPrimitiveState primitiveState;
        
    public:
        RHIGraphicsPipelineCreateInfo();

        ~RHIGraphicsPipelineCreateInfo();

        void AddShaderStage( const RHIShaderStage* shaderStage );

        void AddColorAttachment( const RHIColorAttachmentState& state );
    };

    class RHIGraphicsPipeline : public RHIObject
    {
    protected:
        RHIGraphicsPipeline() {}
    public:
        virtual ~RHIGraphicsPipeline() {};
    };
}