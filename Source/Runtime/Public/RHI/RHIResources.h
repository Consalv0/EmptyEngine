#pragma once

#include "Core/Collections.h"
#include "Math/CoreMath.h"
#include "Core/Name.h"

namespace EE
{
    class RHIObject
    {
    public:
        EE_CLASSNOCOPY( RHIObject )

    protected:
        RHIObject() {};
        virtual ~RHIObject() = default;

    public:
        virtual bool IsValid() const = 0;
    };

    class RHIResource : public RHIObject
    {
    public:
        EE_CLASSNOCOPY( RHIResource )

    protected:
        RHIResource() {};
        virtual ~RHIResource() = default;
    };

    class RHISurface : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHISurface )

    protected:
        RHISurface() {};

    public:
        virtual ~RHISurface() = default;
    };

    struct RHITextureCreateInfo
    {
        ETextureType type = TextureType_Texture2D;
        UIntVector3 extents = { 0, 0, 0 };
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
    };

    class RHITexture : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHITexture )

    protected:
        RHITexture() {}

    public:
        virtual ~RHITexture() = default;

        virtual const UIntVector3& GetExtents() const = 0;

        virtual const EPixelFormat& GetFormat() const = 0;
    };

    class RHIFence : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHIFence )

    protected:
        RHIFence() {};

    public:
        virtual ~RHIFence() = default;

        virtual bool IsSignaled() const = 0;

        virtual void Reset() const = 0;

        virtual void Wait( uint64 timeout ) const = 0;
    };

    class RHISemaphore : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHISemaphore )

    protected:
        RHISemaphore() {}

    public:
        virtual ~RHISemaphore() = default;
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
    public:
        EE_CLASSNOCOPY( RHISwapChain )

    protected:
        RHISwapChain() {};

    public:
        virtual ~RHISwapChain() = default;
    };

    class RHIInstance : public RHIObject
    {
    public:
        EE_CLASSNOCOPY( RHIInstance )

    protected:
        RHIInstance() {}

    public:
        virtual ~RHIInstance() = default;
    };

    struct RHIShaderStageCreateInfo
    {
        EShaderStageFlagsBit stage;
        EShaderFormat format;
        size_t codeLength;
        const void* code;
        const NChar* entryPoint;
    };

    class RHIShaderStage : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHIShaderStage )
    
    public:
        EShaderStageFlagsBit stage = ShaderStage_Unknown;

    protected:
        RHIShaderStage( EShaderStageFlagsBit stage ) : stage( stage ) {}

    public:
        virtual ~RHIShaderStage() = default;

        FORCEINLINE const EShaderStageFlagsBit& GetStage() const { return stage; }

        virtual const NChar* GetEntryPoint() const = 0;
    };

    struct RHICommandBufferCreateInfo
    {
    };

    class RHICommandBuffer : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHICommandBuffer )

    protected:
        RHICommandBuffer() {};

    public:
        virtual ~RHICommandBuffer() = default;

        virtual void Begin() const = 0;

        virtual void End() const = 0;

        virtual void BindGraphicsPipeline( const class RHIGraphicsPipeline* pipeline ) const = 0;

        virtual void BindBindingsGroup( const RHIGraphicsPipeline* pipeline, const class RHIBindGroup* bindGroup, const uint32& dynamicOffsetsCount, const uint32* dynamicOffests ) const = 0;

        virtual void BindVertexBuffer( const class RHIBuffer* buffer, uint32 bindIndex, uint64 offset ) const = 0;

        virtual void BindIndexBuffer( const class RHIBuffer* buffer, uint64 offset ) const = 0;

        virtual void SetCullMode( const ECullModeFlags& cull ) const = 0;

        virtual void SetFrontFace( const EFaceWinding& frontFace ) const = 0;

        virtual void SetViewport( Box2f viewport, float minDepth, float maxDepth ) const = 0;

        virtual void SetScissor( IntBox2 scissor ) const = 0;

        virtual void TransitionTexture( const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer, const ETextureLayout from, const ETextureLayout to ) const = 0;

        virtual void ClearColor( Vector3f color, const RHITexture* texture, uint32 mipLevel, uint32 arrayLayer ) const = 0;

        virtual void DrawIndexed( uint32 indexCount, uint32 instanceCount, uint32 firstIndex, uint32 vertexOffset, uint32 firstInstance ) const = 0;

        virtual void Draw( uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance ) const = 0;
    };

    class RHIPresentContext : public RHIObject
    {
    public:
        EE_CLASSNOCOPY( RHIPresentContext )

    protected:
        uint32 currentFrameIndex;

    protected:
        RHIPresentContext() : currentFrameIndex( 0 ) {};

    public:
        virtual ~RHIPresentContext() = default;

        virtual const RHITexture* GetBackbuffer() const = 0;

        virtual void SubmitRenderCommandBuffer( EPipelineStage stage ) const = 0;

        virtual const RHICommandBuffer* GetRenderCommandBuffer() const = 0;

        virtual const RHIFence* GetRenderFence() const = 0;

        virtual void AquireBackbuffer( uint64 timeout ) = 0;

        FORCEINLINE const uint32& CurrentFrameIndex() const { return currentFrameIndex; }

        virtual const EPixelFormat& GetFormat() const = 0;

        virtual void Present() = 0;

    protected:
        friend class DynamicRHI;
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
    public:
        EE_CLASSNOCOPY( RHIQueue )

    protected:
        RHIQueue() {};

    public:
        virtual ~RHIQueue() = default;

        virtual void SubmitCommandBuffer( const RHICommandBuffer* commandBuffer, const RHIQueueSubmitInfo& info ) = 0;
    };

    struct RHIDeviceLimits
    {
    public:
        uint64 minUniformBufferOffsetAlignment;
    };

    class RHIDevice : public RHIObject
    {
    public:
        EE_CLASSNOCOPY( RHIDevice )

    protected:
        RHIDevice() {}

    public:
        virtual ~RHIDevice() = default;

        virtual RHIQueue* GetGraphicsQueue() const = 0;

        virtual RHIQueue* GetPresentQueue() const = 0;

        virtual const RHIDeviceLimits& GetLimits() const = 0;
    };

    struct RHIBufferCreateInfo
    {
        uint64 size = 0;
        uint64 aligment = 0;
        uint64 offset = 0;
        EBufferUsageFlags usage = BufferUsage_None;
        ESharingMode sharing = SharingMode_Default;
    };

    class RHIBuffer : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHIBuffer )

    protected:
        RHIBuffer() {}

    public:
        virtual ~RHIBuffer() = default;

        virtual uint64 GetSize() const = 0;
        
        virtual uint64 GetOffset() const = 0;

        virtual uint64 GetAligment() const = 0;

        virtual void UploadData( const void* data, uint64 offset, uint64 size ) const = 0;
    };

    class RHIVertexBuffer : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHIVertexBuffer )

    protected:
        RHIVertexBuffer() {}

    public:
        virtual ~RHIVertexBuffer() = 0;
    };

    struct RHISamplerCreateInfo
    {
        EFilterMode filter = FilterMode_MinMagNearest;
        ESamplerAddressMode addressU = SamplerAdressMode_Clamp;
        ESamplerAddressMode addressV = SamplerAdressMode_Clamp;
        ESamplerAddressMode addressW = SamplerAdressMode_Clamp;
        float mipLODBias = 0.0F;
        uint32 maxAnisotropy = 0;
        ECompareOperation compareOperation = CompareOperation_Always;
        ESamplerBorder border = SamplerBorder_BlackTransparent;
        float minLOD = 0.0F;
        float maxLOD = FLT_MAX;
    };

    class RHISampler : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHISampler )

    protected:
        RHISampler() {}

    public:
        virtual ~RHISampler() = default;
    };

    class RHIRenderPass : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHIRenderPass )

    protected:
        RHIRenderPass() {}
    
    public:
        virtual ~RHIRenderPass() = default;

        virtual void SetAttachments( uint32 attachmentCount, const RHITexture** texture ) = 0;

        virtual void SetDrawArea( const IntBox2& extent ) = 0;

        virtual void SetClearValues( const Vector4f& clearColor, const float& clearDepth, const uint32& clearStencil ) = 0;

        virtual void BeginRenderPass( const RHICommandBuffer* cmd ) = 0;

        virtual void EndRenderPass( const RHICommandBuffer* cmd ) = 0;
    };

    struct RHIResourceBinding
    {
        uint8 index;
        const RHIResource* resource;
        EBindingType type;
        EShaderStageFlags shaderVisibility;
        uint64 bufferRange = 0;
    };

    struct RHIBindGroupCreateInfo
    {
        uint8 layoutIndex;
        TArray<RHIResourceBinding> bindings;

    public:
        void AddResourceBinding( const RHIResourceBinding& resourceBinding );
    };

    class RHIBindLayout : public RHIObject
    {
    public:
        EE_CLASSNOCOPY( RHIBindLayout )

    protected:
        RHIBindLayout() {}

    public:
        virtual ~RHIBindLayout() = default;
    };

    class RHIBindGroup : public RHIResource
    {
    public:
        EE_CLASSNOCOPY( RHIBindGroup )

    protected:
        RHIBindGroup() {}

    public:
        virtual ~RHIBindGroup() = default;

        virtual const RHIBindLayout& GetBindLayout() const = 0;
    };

    struct RHIAttachmentDescription
    {
        EPixelFormat format = PixelFormat_Unknown;
        ESampleCountFlagsBit sampleCount = SampleCount_1_Bit;
        ETextureOperation colorLoadOperation = TextureOperation_DontCare;
        ETextureOperation colorStoreOperation = TextureOperation_DontCare;
        ETextureOperation depthLoadOperation = TextureOperation_DontCare;
        ETextureOperation depthStoreOperation = TextureOperation_DontCare;
        ETextureOperation stencilLoadOperation = TextureOperation_DontCare;
        ETextureOperation stencilStoreOperation = TextureOperation_DontCare;
        ETextureLayout initialLayout = TextureLayout_Undefined;
        ETextureLayout finalLayout = TextureLayout_Undefined;
    };

    struct RHIAttachmentReference
    {
        uint32 atachmentIndex = 0;
        ETextureLayout textureLayout = TextureLayout_Undefined;
    };

    struct RHIRenderSubpassDescription
    {
        TArray<RHIAttachmentReference> colorAttachmentReferences;
        TArray<RHIAttachmentReference> inputAttachmentReferences;
        RHIAttachmentReference depthAttachment;
        bool usingDepth;

    public:
        void AddInputAttachment( uint32 atachmentIndex );

        void AddColorAttachment( uint32 atachmentIndex, ETextureLayout layout );

        void AddDepthAttachment( uint32 atachmentIndex, ETextureLayout layout );
    };

    struct RHIRenderPassCreateInfo
    {
    public:
        TArray<RHIRenderSubpassDescription> subpasses;
        TArray<RHIAttachmentDescription> attachments;

    public:
        void AddRenderSubpass( const RHIRenderSubpassDescription& subpass );

        void AddAttachment( const RHIAttachmentDescription& attachment );
    };

    struct RHIVertexAttribute
    {
        EVertexFormat format = VertexFormat_Unknown;
        uint32 offset = 0;
        Name semanticName;
        uint8 semanticIndex;
    };

    struct RHIVertexBufferLayout
    {
        EVertexStepMode stepMode = VertexStepMode_Vertex;
        uint32 stride = 0;
        TArray<RHIVertexAttribute> attributes;
    };

    struct RHIVertexState
    {
        TArray<RHIVertexBufferLayout> bufferLayouts;
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
        EFaceWinding frontFace = FaceWinding_Clockwise;
        // Clip or clamp depth
        bool depthClip = false;
    };

    struct RHIStencilFaceState
    {
        ECompareOperation compareOperation = CompareOperation_Always;
        EStencilOperation failOperation = StencilOperation_Keep;
        EStencilOperation depthFailOperation = StencilOperation_Keep;
        EStencilOperation passOperation = StencilOperation_Keep;
        uint32 compareMask = 0u;
        uint32 writeMask = 0u;
        uint32 reference = UINT32_MAX;
    };

    struct RHIDepthStencilState
    {
        bool depthEnabled = false;
        bool depthWriteEnabled = false;
        bool stencilEnabled = false;
        EPixelFormat format = PixelFormat_MAX;
        ECompareOperation depthCompareOperation = CompareOperation_GreaterEqual;
        int32 depthBias = 0;
        float depthBiasSlopeScale = 0.0F;
        float depthBiasClamp = 0.0F;
        RHIStencilFaceState stencilFront = RHIStencilFaceState();
        RHIStencilFaceState stencilBack = RHIStencilFaceState();
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

    struct RHIGraphicsPipelineCreateInfo
    {
        RHIShaderStageAttachment vertexShader;
        RHIShaderStageAttachment fragmentShader;
        RHIShaderStageAttachment geometryShader;
        RHIShaderStageAttachment domainShader;
        RHIShaderStageAttachment hullShader;

        TArray<RHIColorAttachmentState> colorAttachments;
        RHIDepthStencilState depthStencilState;
        TArray<const RHIBindLayout*> bindLayouts;
        RHIRenderPass* renderpass;
        uint32 subpassIndex;
        RHIVertexState vertexState;
        RHIPrimitiveState primitiveState;
        
    public:
        void AddShaderStage( const RHIShaderStage* shaderStage );

        void AddColorAttachment( const RHIColorAttachmentState& state );
        
        void AddBindLayout( const RHIBindLayout& bindLayout );
    };

    class RHIGraphicsPipeline : public RHIObject
    {
    public:
        EE_CLASSNOCOPY( RHIGraphicsPipeline )

    protected:
        RHIGraphicsPipeline() {}

    public:
        virtual ~RHIGraphicsPipeline() = default;
    };
}