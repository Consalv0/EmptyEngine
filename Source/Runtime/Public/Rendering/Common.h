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

    struct Viewport
    {
        float x;
        float y;
        float width;
        float height;
        float minDepth;
        float maxDepth;
    };

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
        ShaderStage_Fragment,
        ShaderStage_Geometry,
        ShaderStage_Compute,
        ShaderStage_Tesselation,
        ShaderStage_TesselationEval,
    };

    enum EShaderFormat
    {
        ShaderFormat_None,
        ShaderFormat_HLSL5,
        ShaderFormat_HLSL6,
        ShaderFormat_GLSL,
    };

    enum EBlendOperation
    {
        BlendOperation_Add,
        BlendOperation_Substract,
        BlendOperation_ReverseSubstract,
        BlendOperation_Min,
        BlendOperation_Max
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

    enum EColorComponentFlags : uint32
    {
        ColorComponent_None =        0,
        ColorComponent_R_Bit =  1 << 0,
        ColorComponent_G_Bit =  1 << 1,
        ColorComponent_B_Bit =  1 << 2,
        ColorComponent_A_Bit =  1 << 3,
        ColorComponent_RGB = ColorComponent_R_Bit | ColorComponent_G_Bit | ColorComponent_B_Bit,
        ColorComponent_RGBA = ColorComponent_RGB | ColorComponent_A_Bit
    };
    ENUM_FLAGS_OPERATORS( EColorComponentFlags );
    
    // Specifying the tiling arrangement of texel blocks in an texture
    enum ETilingMode
    {
        TilingMode_Default,
        TilingMode_Optimal,
        TilingMode_Linear
    };

    enum EUsageModeFlags : uint32
    {
        UsageMode_None =            0,
        UsageMode_Color_Bit =       1 << 0,
        UsageMode_Stencil_Bit =     1 << 1,
        UsageMode_Depth_Bit =       1 << 2,
        UsageMode_Sampled_Bit =     1 << 3,
        UsageMode_Storage_Bit =     1 << 4,

        UsageMode_DepthStencil = UsageMode_Depth_Bit | UsageMode_Stencil_Bit,
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
        BufferUsage_None =                  0,
        BufferUsage_SourceCopy_Bit =        1 << 1,
        BufferUsage_StreamOutput_Bit =      1 << 2,
        BufferUsage_Index_Bit =             1 << 3,
        BufferUsage_Vertex_Bit =            1 << 4,
        BufferUsage_Uniform_Bit =           1 << 5,
        BufferUsage_Storage_Bit =           1 << 6,
        BufferUsage_Indirect_Bit =          1 << 7,
        BufferUsage_ShaderResource_Bit =    1 << 8,
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

    enum ECullModeFlags : uint8
    {
        CullMode_None =         0,
        CullMode_Front_Bit =    1 << 0,
        CullMode_Back_Bit =     1 << 1,
        CullMode_All = CullMode_Front_Bit | CullMode_Back_Bit
    };
    ENUM_FLAGS_OPERATORS( ECullModeFlags );

    enum EColorSpace
    {
        ColorSpace_Unknown,
        ColorSpace_Linear,
        ColorSpace_sRGB,
    };

    enum EPresentMode
    {
        PresentMode_Inmediate,
        PresentMode_VSync
    };

    enum EPixelFormat
    {
        PixelFormat_Unknown,
        PixelFormat_R8_SINT,
        PixelFormat_R8_UINT,
        PixelFormat_R8_SNORM,
        PixelFormat_R8_UNORM,
        PixelFormat_R16_SINT,
        PixelFormat_R16_UINT,
        PixelFormat_R16_SNORM,
        PixelFormat_R16_UNORM,
        PixelFormat_R32_SINT,
        PixelFormat_R32_UINT,
        PixelFormat_R32_SNORM,
        PixelFormat_R32_UNORM,
        PixelFormat_R8G8_SINT,
        PixelFormat_R8G8_UINT,
        PixelFormat_R8G8_SNORM,
        PixelFormat_R8G8_UNORM,
        PixelFormat_R16G16_SINT,
        PixelFormat_R16G16_UINT,
        PixelFormat_R16G16_SNORM,
        PixelFormat_R16G16_UNORM,
        PixelFormat_R32G32_UINT,
        PixelFormat_R5G6B5_UINT,
        PixelFormat_R8G8B8A8_SINT,
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

    enum ERasterMode
    {
        RasterMode_Point,
        RasterMode_Wireframe,
        RasterMode_Solid,
    };

    enum EFaceWinding
    {
        FaceWinding_Clockwise,
        FaceWinding_CounterClokwise
    };

    enum EFilterMode
    {
        FilterMode_MinMagNearest,
        FilterMode_MinMagLinear,
        FilterMode_MinLinearMagNearest,
        FilterMode_MinNearestMagLinear,
    };

    enum ETextureLayout
    {
        TextureLayout_Undefined,
        TextureLayout_General,
        TextureLayout_TransferSRC,
        TextureLayout_TransferDST,
        TextureLayout_ColorAttachment,
        TextureLayout_DepthAttachment,
        TextureLayout_StencilAttachment,
        TextureLayout_Present
    };

    enum ETopologyMode
    {
        TopologyMode_PointList,
        TopologyMode_LineList,
        TopologyMode_LineStrip,
        TopologyMode_TriangleList,
        TopologyMode_TriangleStrip,
        TopologyMode_TriangleFan,
    };

    enum EClearFlags : uint32
    {
        ClearFlags_None =           0,
        ClearFlags_Color_Bit =      1 << 0,
        ClearFlags_Depth_Bit =      1 << 1,
        ClearFlags_Stencil_Bit =    1 << 2
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