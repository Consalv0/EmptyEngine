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
        ShaderStage_Tesselation,
        ShaderStage_TesselationEval,
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

    enum EUsageModeFlags : uint32
    {
        UsageMode_None          = 0,
        UsageMode_Color         = 1 << 0,
        UsageMode_Stencil       = 1 << 1,
        UsageMode_Depth         = 1 << 2,
        UsageMode_Sampled       = 1 << 3,
        UsageMode_Storage       = 1 << 4,

        UsageMode_DepthStencil = UsageMode_Depth | UsageMode_Stencil,
    };
    ENUM_FLAGS_OPERATORS( EUsageModeFlags );

    enum ESharingMode
    {
        SharingMode_Default,
        SharingMode_Dynamic,
        SharingMode_Static,
    };

    enum EBufferUsageFlags : uint32
    {
        BufferUsage_None =              0,
        BufferUsage_SourceCopy =        1 << 1,
        BufferUsage_StreamOutput =      1 << 2,
        BufferUsage_Index =             1 << 3,
        BufferUsage_Vertex =            1 << 4,
        BufferUsage_Uniform =           1 << 5,
        BufferUsage_Storage =           1 << 6,
        BufferUsage_Indirect =          1 << 7,
        BufferUsage_ShaderResource =    1 << 8,
    };
    ENUM_FLAGS_OPERATORS( EBufferUsageFlags );

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

    enum EColorSpace
    {
        ColorSpace_Unknown,
        ColorSpace_Linear,
        ColorSpace_sRGB,
    };

    enum EPixelFormat
    {
        PixelFormat_Unknown,
        PixelFormat_R8_UINT,
        PixelFormat_R8_SNORM,
        PixelFormat_R8_UNORM,
        PixelFormat_R16_UINT,
        PixelFormat_R16_SNORM,
        PixelFormat_R16_UNORM,
        PixelFormat_R32_UINT,
        PixelFormat_R32_SNORM,
        PixelFormat_R32_UNORM,
        PixelFormat_R8G8_UINT,
        PixelFormat_R8G8_SNORM,
        PixelFormat_R8G8_UNORM,
        PixelFormat_R16G16_UINT,
        PixelFormat_R16G16_SNORM,
        PixelFormat_R16G16_UNORM,
        PixelFormat_R32G32_UINT,
        PixelFormat_R5G6B5_UINT,
        PixelFormat_R8G8B8A8_UINT,
        PixelFormat_R8G8B8A8_SNORM,
        PixelFormat_R8G8B8A8_UNORM,
        PixelFormat_R4G4B4A4_UINT,
        PixelFormat_A2R10G10B10_UNORM,
        PixelFormat_R16G16B16A16_SNORM,
        PixelFormat_DepthComponent24,
        PixelFormat_DepthStencil,
        PixelFormat_ShadowDepth,
        PixelFormat_MAX = PixelFormat_ShadowDepth + 1,
    };

    struct PixelFormatInfo
    {
        const WChar* name;
        int size;
        int channels;
        bool supported;
        EPixelFormat format;
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

    enum EClearFlags : uint32
    {
        ClearFlags_None     = 0,
        ClearFlags_Color    = 1 << 0,
        ClearFlags_Depth    = 1 << 1,
        ClearFlags_Stencil  = 1 << 2
    };
    ENUM_FLAGS_OPERATORS( EClearFlags );

    enum ESamplerBorder
    {
        SamplerBorder_BlackTransparent,
        SamplerBorder_Black,
        SamplerBorder_White
    };

    enum ERenderingResourceType
    {
        RenderingResourceType_Surface,
        RenderingResourceType_Swapchain,
        RenderingResourceType_Buffer,
        RenderingResourceType_Texture,
        RenderingResourceType_Shader,
        RenderingResourceType_Unknown
    };

    enum ECommandQueueType
    {
        CommandQueue_Graphics,
        CommandQueue_Present,
        CommandQueue_Copy
    };

    enum EPipelineStage
    {
        PipelineStage_None,
        PipelineStage_OutputColor,
        PipelineStage_Top
    };

    enum class EDynamicRHI
    {
        Vulkan
    };
}