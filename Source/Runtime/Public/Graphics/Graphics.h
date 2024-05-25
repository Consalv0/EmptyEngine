#pragma once

namespace EE
{
	typedef struct UCharRed  { typedef unsigned char Range; static constexpr unsigned char Channels = 1; Range R; } UCharRed;
	typedef struct UCharRG   { typedef unsigned char Range; static constexpr unsigned char Channels = 2; Range R; Range G; } UCharRG;
	typedef struct UShortRG  { typedef unsigned char Range; static constexpr unsigned char Channels = 2; Range R; Range G; } UShortRG;
	typedef struct UCharRGB  { typedef unsigned char Range; static constexpr unsigned char Channels = 3; Range R; Range G; Range B; } UCharRGB;
    typedef struct UCharRGBA { typedef unsigned char Range; static constexpr unsigned char Channels = 4; Range R; Range G; Range B; Range A; } UCharRGBA;
	typedef struct FloatRed  { typedef float Range; static constexpr unsigned char Channels = 1; Range R; } FloatRed;
	typedef struct FloatRG   { typedef float Range; static constexpr unsigned char Channels = 2; Range R; Range G; } FloatRG;
	typedef struct FloatRGB  { typedef float Range; static constexpr unsigned char Channels = 3; Range R; Range G; Range B; } FloatRGB;
    typedef struct FloatRGBA { typedef float Range; static constexpr unsigned char Channels = 4; Range R; Range G; Range B; Range A; } FloatRGBA;

    enum ETextureType
    {
        TextureType_None,
        TextureType_Texture1D,
        TextureType_Texture2D,
        TextureType_Texture3D,
        TextureType_Cubemap
    };

    enum ECubemapFace
    {
        Cubemap_Front,
        Cubemap_Back,
        Cubemap_Right,
        Cubemap_Left,
        Cubemap_Up,
        Cubemap_Down
    };

    enum EShaderStage
    {
        ShaderStage_Unknown,
        ShaderStage_Vertex,
        ShaderStage_Geometry,
        ShaderStage_Pixel,
        ShaderStage_Compute
    };

    enum EShaderFormat
    {
        ShaderFormat_None,
        ShaderFormat_HLSL5,
        ShaderFormat_HLSL6,
        ShaderFormat_GLSL,
    };

    enum EBlendFactor
    {
        BlendFactor_None,
        BlendFactor_Zero,
        BlendFactor_One,
        BlendFactor_SrcColor,
        BlendFactor_SrcAlpha,
        BlendFactor_DstAlpha,
        BlendFactor_DstColor,
        BlendFactor_OneMinusSrcColor,
        BlendFactor_OneMinusSrcAlpha,
        BlendFactor_OneMinusDstAlpha,
        BlendFactor_OneMinusDstColor,
    };
    
    // Specifying the tiling arrangement of texel blocks in an texture
    enum ETilingMode
    {
        TilingMode_Default,
        TilingMode_Optimal,
        TilingMode_Linear
    };

    enum EUsageMode
    {
        UsageMode_Sampled,
        UsageMode_Storage,
        UsageMode_Color,
        UsageMode_DepthStencil,
    };

    enum ESharingMode
    {
        SharingMode_Default,
        SharingMode_Dynamic,
        SharingMode_Static,
    };

    enum EComparisonFunction
    {
        ComparisonFuntion_Never,
        ComparisonFuntion_Less,
        ComparisonFuntion_Equal,
        ComparisonFuntion_LessEqual,
        ComparisonFuntion_Greater,
        ComparisonFuntion_NotEqual,
        ComparisonFuntion_GreaterEqual,
        ComparisonFuntion_Always
    };

    enum EStencilFunction
    {
        StencilFunction_Never,
        StencilFunction_Less,
        StencilFunction_Equal,
        StencilFunction_LessEqual,
        StencilFunction_Greater,
        StencilFunction_NotEqual,
        StencilFunction_GreaterEqual,
        StencilFunction_Always
    };

    enum EStencilOperation
    {
        StencilOperation_Keep,
        StencilOperation_Zero,
        StencilOperation_Replace,
        StencilOperation_Increment,
        StencilOperation_IncrementLoop,
        StencilOperation_Decrement,
        StencilOperation_DecrementLoop,
        StencilOperation_Invert
    };

    enum EImageLayout
    {
        ImageLayout_Undefined,
        ImageLayout_RenderTarget,
        ImageLayout_DepthStencil,
        ImageLayout_DepthStencil_Readonly,
        ImageLayout_ShaderResource,
        ImageLayout_ShaderResourceCompute,
        ImageLayout_UnorderedAccess,
        ImageLayout_CopySource,
        ImageLayout_CopyDest,
    };

    enum ECullMode
    {
        CullMode_None,
        CullMode_Front,
        CullMode_Back
    };

    enum EPixelFormat
    {
        PixelFormat_Unknown,
        PixelFormat_R8,
        PixelFormat_R32F,
        PixelFormat_R8G8,
        PixelFormat_RG32F,
        PixelFormat_RG16F,
        PixelFormat_R8G8B8,
        PixelFormat_RGB32F,
        PixelFormat_RGB16F,
        PixelFormat_R8G8B8A8,
        PixelFormat_RGBA16_UShort,
        PixelFormat_RGBA32F,
        PixelFormat_DepthComponent24,
        PixelFormat_DepthStencil,
        PixelFormat_ShadowDepth,
        Private_Num = PixelFormat_ShadowDepth + 1,
    };

    enum ESamplerAddressMode
    {
        SamplerAdressMode_Repeat,
        SamplerAdressMode_Mirror,
        SamplerAdressMode_Clamp,
        SamplerAdressMode_Border
    };

    enum ERasterizerFillMode
    {
        FillMode_Point,
        FillMode_Wireframe,
        FillMode_Solid,
    };

    enum EFilterMode
    {
        FilterMode_MinMagNearest,
        FilterMode_MinMagLinear,
        FilterMode_MinLinearMagNearest,
        FilterMode_MinNearestMagLinear,
    };

    enum EDrawMode
    {
        DrawMode_Points,
        DrawMode_Lines,
        DrawMode_Triangles
    };

    enum ESamplerBorder
    {
        SamplerBorder_BlackTransparent,
        SamplerBorder_Black,
        SamplerBorder_White
    };

    enum EGraphicsDeviceCapability
    {
        GraphicsDeviceCapability_Tesselation = 1 << 0
    };

    enum EGraphicsResourceType
    {
        GraphicsResourceType_Surface,
        GraphicsResourceType_Swapchain,
        GraphicsResourceType_Buffer,
        GraphicsResourceType_Texture,
        GraphicsResourceType_Unknown
    };

    enum ECommandQueueType
    {
        ComandQueue_Graphics,
        ComandQueue_Compute,
        ComandQueue_Count,
    };

    enum EWindowMode
    {
        WindowMode_Windowed = 0,
        WindowMode_FullScreen = 1
    };

    enum EWindowOption
    {
        WindowOption_None = 0,
        WindowOption_Borderless = 1 << 0,
        WindowOption_Resizable = 1 << 1,
        WindowOption_AlwaysOnTop = 1 << 2,
        WindowOption_SkipTaskbar = 1 << 3,
    };

    enum EWindowGraphicsAPI
    {
        WindowGraphicsAPI_Other = 0,
        WindowGraphicsAPI_OpenGL,
        WindowGraphicsAPI_Vulkan
    };

    struct GraphicsDeviceObject
    {
        std::shared_ptr<void> internalState;
        FORCEINLINE bool IsValid() const { return internalState.get() != nullptr; }
    };

    struct GraphicsDeviceResource : public GraphicsDeviceObject
    {
        EGraphicsResourceType type = GraphicsResourceType_Unknown;
        FORCEINLINE bool IsTexture() const { return type == GraphicsResourceType_Texture; }
        FORCEINLINE bool IsBuffer() const { return type == GraphicsResourceType_Buffer; }
        FORCEINLINE bool IsSwapChain() const { return type == GraphicsResourceType_Swapchain; }
        FORCEINLINE bool IsSurface() const { return type == GraphicsResourceType_Surface; }
    };

    struct WindowSurfaceDescription
    {
        class Window* window;
    };

    struct WindowSurface : public GraphicsDeviceResource
    {
        WindowSurfaceDescription description;
        FORCEINLINE const WindowSurfaceDescription& GetDescription() const { return description; }
    };

    struct SwapChainDescription
    {
        uint32 width = 0;
        uint32 height = 0;
        uint32 buffercount = 2;
        EPixelFormat format = PixelFormat_RGBA16_UShort;
        bool fullscreen = false;
        bool vsync = true;
        Vector4 clearcolor = { 0,0,0,1 };
        bool allow_hdr = true;
    };

    struct SwapChain : public GraphicsDeviceResource
    {
        SwapChainDescription description;
        FORCEINLINE const SwapChainDescription& GetDescription() const { return description; }
    };

    struct GPUBufferDescription
    {
        uint32 byteWidth = 0;
        ESharingMode sharing = SharingMode_Default;
        uint32 bindFlags = 0;
        uint32 cpuAccessFlags = 0;
        uint32 miscFlags = 0;
        uint32 structureByteStride = 0; // needed for typed and structured buffer types!
        EPixelFormat format = PixelFormat_Unknown; // only needed for typed buffer!
    };

    struct Shader : public GraphicsDeviceObject
    {
        EShaderStage stage = ShaderStage_Unknown;
    };

    struct Buffer : public GraphicsDeviceObject
    {
        GPUBufferDescription description;
        FORCEINLINE const GPUBufferDescription& GetDescription() const { return description; }
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

    struct Texture : public GraphicsDeviceResource
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

    struct Sampler : public GraphicsDeviceObject
    {
        SamplerDescription description;
        FORCEINLINE const SamplerDescription& GetDescription() const { return description; }
    };

    struct PipelineStateDesccription
    {
        const Shader* vertex = nullptr;
        const Shader* pixel = nullptr;
        const Shader* hull = nullptr;
        const Shader* ds = nullptr;
        const Shader* geometry = nullptr;
        const Shader* ms = nullptr;
        const Shader* as = nullptr;
        // const BlendState* bs = nullptr;
        // const RasterizerState* rs = nullptr;
        // const DepthStencilState* dss = nullptr;
        // const InputLayout* il = nullptr;
        // PRIMITIVETOPOLOGY		pt = TRIANGLELIST;
        uint32				sampleMask = 0xFFFFFFFF;
    };

    struct SubresourceData
    {
        const void* memoryPointer = nullptr;
        uint32 memoryPitch = 0;
        uint32 memorySlicePitch = 0;
    };

    struct PixelFormatInfo
    {
        const WChar* name;
        int             size;
        int             channels;
        // Is supported on the current platform
        bool            supported;
        EPixelFormat    format;
    };
}