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
        BufferUsage_MapWite_Bit =          1 << 3,
        BufferUsage_MapRead_Bit =          1 << 4,
        BufferUsage_Index_Bit =             1 << 5,
        BufferUsage_Vertex_Bit =            1 << 6,
        BufferUsage_Uniform_Bit =           1 << 7,
        BufferUsage_Storage_Bit =           1 << 8,
        BufferUsage_Indirect_Bit =          1 << 9,
        BufferUsage_ShaderResource_Bit =    1 << 10,
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
    
    enum ESampleCountFlagsBit
    {
        SampleCount_1_Bit = 1 << 0,
        SampleCount_2_Bit = 1 << 1,
        SampleCount_4_Bit = 1 << 2,
        SampleCount_8_Bit = 1 << 3,
        SampleCount_16_Bit = 1 << 4,
        SampleCount_32_Bit = 1 << 5,
        SampleCount_64_Bit = 1 << 6,
    };
    typedef uint32 ESampleCountFlags;

    enum ETextureOperation
    {
        TextureOperation_DontCare,
        TextureOperation_Clear,
        TextureOperation_Store,
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

    enum EVertexStepMode
    {
        VertexStepMode_Vertex,
        VertexStepMode_Instance
    };

    enum ECullModeFlags : uint8
    {
        CullMode_None =         0,
        CullMode_Front_Bit =    1 << 0,
        CullMode_Back_Bit =     1 << 1,
        CullMode_All = CullMode_Front_Bit | CullMode_Back_Bit
    };
    ENUM_FLAGS_OPERATORS( ECullModeFlags );

    enum EPresentMode
    {
        PresentMode_Inmediate,
        PresentMode_VSync
    };

    enum EColorSpace
    {
        ColorSpace_Unknown,
        ColorSpace_Linear,
        ColorSpace_sRGB,
        ColorSpace_HDR,
        ColorSpace_HDR_SCRGB,
        ColorSpace_HDR10,
        ColorSpace_HDR10_SCRGB,
        ColorSpace_HDR10_2084,
    };

    // UNORM is a float in the range of [0, 1].
    // SNORM is the same but in the range of [-1, 1]
    // USCALED is the unsigned integer value converted to float
    // SSCALED is the integer value converted to float
    // UINT is an unsigned integer
    // SINT is a signed integer
    // SRGB is in a non-linear space float
    // BLOCK is compressed format
    enum EPixelFormat
    {
        PixelFormat_Unknown,
        PixelFormat_R4G4_UNORM_PACK8,
        PixelFormat_R4G4B4A4_UNORM_PACK16,
        PixelFormat_B4G4R4A4_UNORM_PACK16, // DXGI_FORMAT_B4G4R4A4_UNORM
        PixelFormat_R5G6B5_UNORM_PACK16,
        PixelFormat_B5G6R5_UNORM_PACK16,   // DXGI_FORMAT_B5G6R5_UNORM
        PixelFormat_R5G5B5A1_UNORM_PACK16,
        PixelFormat_B5G5R5A1_UNORM_PACK16, // DXGI_FORMAT_B5G5R5A1_UNORM
        PixelFormat_A1R5G5B5_UNORM_PACK16,
        PixelFormat_R8_UNORM,              // DXGI_FORMAT_R8_UNORM
        PixelFormat_R8_SNORM,              // DXGI_FORMAT_R8_SNORM
        PixelFormat_R8_USCALED,            
        PixelFormat_R8_SSCALED,            
        PixelFormat_R8_UINT,               // DXGI_FORMAT_R8_UINT
        PixelFormat_R8_SINT,               // DXGI_FORMAT_R8_SINT
        PixelFormat_R8_SRGB,
        PixelFormat_R8G8_UNORM,            // DXGI_FORMAT_R8G8_UNORM
        PixelFormat_R8G8_SNORM,            // DXGI_FORMAT_R8G8_SNORM
        PixelFormat_R8G8_USCALED,          
        PixelFormat_R8G8_SSCALED,          
        PixelFormat_R8G8_UINT,             // DXGI_FORMAT_R8G8_UINT
        PixelFormat_R8G8_SINT,             // DXGI_FORMAT_R8G8_SINT
        PixelFormat_R8G8_SRGB,
        PixelFormat_R8G8B8_UNORM,
        PixelFormat_R8G8B8_SNORM,
        PixelFormat_R8G8B8_USCALED,
        PixelFormat_R8G8B8_SSCALED,
        PixelFormat_R8G8B8_UINT,
        PixelFormat_R8G8B8_SINT,
        PixelFormat_R8G8B8_SRGB,
        PixelFormat_B8G8R8_UNORM,
        PixelFormat_B8G8R8_SNORM,
        PixelFormat_B8G8R8_USCALED,
        PixelFormat_B8G8R8_SSCALED,
        PixelFormat_B8G8R8_UINT,
        PixelFormat_B8G8R8_SINT,
        PixelFormat_B8G8R8_SRGB,
        PixelFormat_R8G8B8A8_UNORM,        // DXGI_FORMAT_R8G8B8A8_UNORM
        PixelFormat_R8G8B8A8_SNORM,        // DXGI_FORMAT_R8G8B8A8_SNORM
        PixelFormat_R8G8B8A8_USCALED,      
        PixelFormat_R8G8B8A8_SSCALED,      
        PixelFormat_R8G8B8A8_UINT,         // DXGI_FORMAT_R8G8B8A8_UINT
        PixelFormat_R8G8B8A8_SINT,         // DXGI_FORMAT_R8G8B8A8_SINT
        PixelFormat_R8G8B8A8_SRGB,         // DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
        PixelFormat_B8G8R8A8_UNORM,        // DXGI_FORMAT_B8G8R8A8_UNORM
        PixelFormat_B8G8R8A8_SNORM,
        PixelFormat_B8G8R8A8_USCALED,
        PixelFormat_B8G8R8A8_SSCALED,
        PixelFormat_B8G8R8A8_UINT,
        PixelFormat_B8G8R8A8_SINT,
        PixelFormat_B8G8R8A8_SRGB,         // DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
        PixelFormat_A8B8G8R8_UNORM_PACK32,
        PixelFormat_A8B8G8R8_SNORM_PACK32,
        PixelFormat_A8B8G8R8_USCALED_PACK32,
        PixelFormat_A8B8G8R8_SSCALED_PACK32,
        PixelFormat_A8B8G8R8_UINT_PACK32,
        PixelFormat_A8B8G8R8_SINT_PACK32,
        PixelFormat_A8B8G8R8_SRGB_PACK32,
        PixelFormat_A2R10G10B10_UNORM_PACK32,    // DXGI_FORMAT_R10G10B10A2_UNORM ?
        PixelFormat_A2R10G10B10_SNORM_PACK32,
        PixelFormat_A2R10G10B10_USCALED_PACK32,
        PixelFormat_A2R10G10B10_SSCALED_PACK32,
        PixelFormat_A2R10G10B10_UINT_PACK32,     // DXGI_FORMAT_R10G10B10A2_UINT ?
        PixelFormat_A2R10G10B10_SINT_PACK32,
        PixelFormat_A2B10G10R10_UNORM_PACK32,
        PixelFormat_A2B10G10R10_SNORM_PACK32,
        PixelFormat_A2B10G10R10_USCALED_PACK32,
        PixelFormat_A2B10G10R10_SSCALED_PACK32,
        PixelFormat_A2B10G10R10_UINT_PACK32,
        PixelFormat_A2B10G10R10_SINT_PACK32,
        PixelFormat_R16_UNORM,             // DXGI_FORMAT_R16_UNORM
        PixelFormat_R16_SNORM,             // DXGI_FORMAT_R16_SNORM
        PixelFormat_R16_USCALED,           
        PixelFormat_R16_SSCALED,           
        PixelFormat_R16_UINT,              // DXGI_FORMAT_R16_UINT
        PixelFormat_R16_SINT,              // DXGI_FORMAT_R16_SINT
        PixelFormat_R16_SFLOAT,            // DXGI_FORMAT_R16_FLOAT
        PixelFormat_R16G16_UNORM,          // DXGI_FORMAT_R16G16_UNORM
        PixelFormat_R16G16_SNORM,          // DXGI_FORMAT_R16G16_SNORM
        PixelFormat_R16G16_USCALED,        
        PixelFormat_R16G16_SSCALED,        
        PixelFormat_R16G16_UINT,           // DXGI_FORMAT_R16G16_UINT
        PixelFormat_R16G16_SINT,           // DXGI_FORMAT_R16G16_SINT
        PixelFormat_R16G16_SFLOAT,         // DXGI_FORMAT_R16G16_FLOAT
        PixelFormat_R16G16B16_UNORM,
        PixelFormat_R16G16B16_SNORM,
        PixelFormat_R16G16B16_USCALED,
        PixelFormat_R16G16B16_SSCALED,
        PixelFormat_R16G16B16_UINT,
        PixelFormat_R16G16B16_SINT,
        PixelFormat_R16G16B16_SFLOAT,
        PixelFormat_R16G16B16A16_UNORM,    // DXGI_FORMAT_R16G16B16A16_UNORM
        PixelFormat_R16G16B16A16_SNORM,    // DXGI_FORMAT_R16G16B16A16_SNORM
        PixelFormat_R16G16B16A16_USCALED, 
        PixelFormat_R16G16B16A16_SSCALED, 
        PixelFormat_R16G16B16A16_UINT,     // DXGI_FORMAT_R16G16B16A16_UINT 
        PixelFormat_R16G16B16A16_SINT,     // DXGI_FORMAT_R16G16B16A16_SINT
        PixelFormat_R16G16B16A16_SFLOAT,   // DXGI_FORMAT_R16G16B16A16_FLOAT
        PixelFormat_R32_UINT,              // DXGI_FORMAT_R32_UINT
        PixelFormat_R32_SINT,              // DXGI_FORMAT_R32_SINT
        PixelFormat_R32_SFLOAT,            // DXGI_FORMAT_R32_FLOAT
        PixelFormat_R32G32_UINT,           // DXGI_FORMAT_R32G32_UINT
        PixelFormat_R32G32_SINT,           // DXGI_FORMAT_R32G32_SINT
        PixelFormat_R32G32_SFLOAT,         // DXGI_FORMAT_R32G32_SFLOAT
        PixelFormat_R32G32B32_UINT,        // DXGI_FORMAT_R32G32B32_UINT
        PixelFormat_R32G32B32_SINT,        // DXGI_FORMAT_R32G32B32_SINT
        PixelFormat_R32G32B32_SFLOAT,      // DXGI_FORMAT_R32G32B32_FLOAT
        PixelFormat_R32G32B32A32_UINT,     // DXGI_FORMAT_R32G32B32A32_UINT
        PixelFormat_R32G32B32A32_SINT,     // DXGI_FORMAT_R32G32B32A32_SINT
        PixelFormat_R32G32B32A32_SFLOAT,   // DXGI_FORMAT_R32G32B32A32_FLOAT
        PixelFormat_R64_UINT,
        PixelFormat_R64_SINT,
        PixelFormat_R64_SFLOAT,
        PixelFormat_R64G64_UINT,
        PixelFormat_R64G64_SINT,
        PixelFormat_R64G64_SFLOAT,
        PixelFormat_R64G64B64_UINT,
        PixelFormat_R64G64B64_SINT,
        PixelFormat_R64G64B64_SFLOAT,
        PixelFormat_R64G64B64A64_UINT,
        PixelFormat_R64G64B64A64_SINT,
        PixelFormat_R64G64B64A64_SFLOAT,
        PixelFormat_B10G11R11_UFLOAT_PACK32,
        PixelFormat_E5B9G9R9_UFLOAT_PACK32,
        PixelFormat_D16_UNORM,             // DXGI_FORMAT_D16_UNORM
        PixelFormat_X8_D24_UNORM_PACK32,
        PixelFormat_D32_SFLOAT,            // DXGI_FORMAT_D32_FLOAT
        PixelFormat_S8_UINT,
        PixelFormat_D16_UNORM_S8_UINT,     
        PixelFormat_D24_UNORM_S8_UINT,     // DXGI_FORMAT_D24_UNORM_S8_UINT
        PixelFormat_D32_SFLOAT_S8_UINT,    // DXGI_FORMAT_D32_FLOAT_S8X24_UINT
        PixelFormat_BC1_RGB_UNORM_BLOCK,   
        PixelFormat_BC1_RGB_SRGB_BLOCK,    
        PixelFormat_BC1_RGBA_UNORM_BLOCK,  // DXGI_FORMAT_BC1_UNORM
        PixelFormat_BC1_RGBA_SRGB_BLOCK,   // DXGI_FORMAT_BC1_UNORM_SRGB
        PixelFormat_BC2_UNORM_BLOCK,       // DXGI_FORMAT_BC2_UNORM
        PixelFormat_BC2_SRGB_BLOCK,        // DXGI_FORMAT_BC2_UNORM_SRGB
        PixelFormat_BC3_UNORM_BLOCK,       // DXGI_FORMAT_BC3_UNORM
        PixelFormat_BC3_SRGB_BLOCK,        // DXGI_FORMAT_BC3_UNORM_SRGB
        PixelFormat_BC4_UNORM_BLOCK,       // DXGI_FORMAT_BC4_UNORM
        PixelFormat_BC4_SNORM_BLOCK,       // DXGI_FORMAT_BC4_SNORM
        PixelFormat_BC5_UNORM_BLOCK,       // DXGI_FORMAT_BC5_UNORM
        PixelFormat_BC5_SNORM_BLOCK,       // DXGI_FORMAT_BC5_SNORM
        PixelFormat_BC6H_UFLOAT_BLOCK,     // DXGI_FORMAT_BC6H_UF16
        PixelFormat_BC6H_SFLOAT_BLOCK,     // DXGI_FORMAT_BC6H_SF16
        PixelFormat_BC7_UNORM_BLOCK,       // DXGI_FORMAT_BC7_UNORM
        PixelFormat_BC7_SRGB_BLOCK,        // DXGI_FORMAT_BC7_UNORM_SRGB
        PixelFormat_ETC2_R8G8B8_UNORM_BLOCK,
        PixelFormat_ETC2_R8G8B8_SRGB_BLOCK,
        PixelFormat_ETC2_R8G8B8A1_UNORM_BLOCK,
        PixelFormat_ETC2_R8G8B8A1_SRGB_BLOCK,
        PixelFormat_ETC2_R8G8B8A8_UNORM_BLOCK,
        PixelFormat_ETC2_R8G8B8A8_SRGB_BLOCK,
        PixelFormat_EAC_R11_UNORM_BLOCK,
        PixelFormat_EAC_R11_SNORM_BLOCK,
        PixelFormat_EAC_R11G11_UNORM_BLOCK,
        PixelFormat_EAC_R11G11_SNORM_BLOCK,
        PixelFormat_ASTC_4x4_UNORM_BLOCK,
        PixelFormat_ASTC_4x4_SRGB_BLOCK,
        PixelFormat_ASTC_5x4_UNORM_BLOCK,
        PixelFormat_ASTC_5x4_SRGB_BLOCK,
        PixelFormat_ASTC_5x5_UNORM_BLOCK,
        PixelFormat_ASTC_5x5_SRGB_BLOCK,
        PixelFormat_ASTC_6x5_UNORM_BLOCK,
        PixelFormat_ASTC_6x5_SRGB_BLOCK,
        PixelFormat_ASTC_6x6_UNORM_BLOCK,
        PixelFormat_ASTC_6x6_SRGB_BLOCK,
        PixelFormat_ASTC_8x5_UNORM_BLOCK,
        PixelFormat_ASTC_8x5_SRGB_BLOCK,
        PixelFormat_ASTC_8x6_UNORM_BLOCK,
        PixelFormat_ASTC_8x6_SRGB_BLOCK,
        PixelFormat_ASTC_8x8_UNORM_BLOCK,
        PixelFormat_ASTC_8x8_SRGB_BLOCK,
        PixelFormat_ASTC_10x5_UNORM_BLOCK,
        PixelFormat_ASTC_10x5_SRGB_BLOCK,
        PixelFormat_ASTC_10x6_UNORM_BLOCK,
        PixelFormat_ASTC_10x6_SRGB_BLOCK,
        PixelFormat_ASTC_10x8_UNORM_BLOCK,
        PixelFormat_ASTC_10x8_SRGB_BLOCK,
        PixelFormat_ASTC_10x10_UNORM_BLOCK,
        PixelFormat_ASTC_10x10_SRGB_BLOCK,
        PixelFormat_ASTC_12x10_UNORM_BLOCK,
        PixelFormat_ASTC_12x10_SRGB_BLOCK,
        PixelFormat_ASTC_12x12_UNORM_BLOCK,
        PixelFormat_ASTC_12x12_SRGB_BLOCK,
        PixelFormat_A4R4G4B4_UNORM_PACK16,
        PixelFormat_A4B4G4R4_UNORM_PACK16,
        PixelFormat_ASTC_4x4_SFLOAT_BLOCK,
        PixelFormat_ASTC_5x4_SFLOAT_BLOCK,
        PixelFormat_ASTC_5x5_SFLOAT_BLOCK,
        PixelFormat_ASTC_6x5_SFLOAT_BLOCK,
        PixelFormat_ASTC_6x6_SFLOAT_BLOCK,
        PixelFormat_ASTC_8x5_SFLOAT_BLOCK,
        PixelFormat_ASTC_8x6_SFLOAT_BLOCK,
        PixelFormat_ASTC_8x8_SFLOAT_BLOCK,
        PixelFormat_ASTC_10x5_SFLOAT_BLOCK,
        PixelFormat_ASTC_10x6_SFLOAT_BLOCK,
        PixelFormat_ASTC_10x8_SFLOAT_BLOCK,
        PixelFormat_ASTC_10x10_SFLOAT_BLOCK,
        PixelFormat_ASTC_12x10_SFLOAT_BLOCK,
        PixelFormat_ASTC_12x12_SFLOAT_BLOCK,
        PixelFormat_PVRTC1_2BPP_UNORM_BLOCK_IMG,
        PixelFormat_PVRTC1_4BPP_UNORM_BLOCK_IMG,
        PixelFormat_PVRTC2_2BPP_UNORM_BLOCK_IMG,
        PixelFormat_PVRTC2_4BPP_UNORM_BLOCK_IMG,
        PixelFormat_PVRTC1_2BPP_SRGB_BLOCK_IMG,
        PixelFormat_PVRTC1_4BPP_SRGB_BLOCK_IMG,
        PixelFormat_PVRTC2_2BPP_SRGB_BLOCK_IMG,
        PixelFormat_PVRTC2_4BPP_SRGB_BLOCK_IMG,
        PixelFormat_R16G16_SFIXED5_NV,
        PixelFormat_A1B5G5R5_UNORM_PACK16_KHR,
        PixelFormat_A8_UNORM_KHR,          // DXGI_FORMAT_A8_UNORM
        // Video encoding formats
        PixelFormatVideo_G8B8G8R8_422_UNORM,
        PixelFormatVideo_B8G8R8G8_422_UNORM,
        PixelFormatVideo_G8_B8_R8_3PLANE_420_UNORM,
        PixelFormatVideo_G8_B8R8_2PLANE_420_UNORM,
        PixelFormatVideo_G8_B8_R8_3PLANE_422_UNORM,
        PixelFormatVideo_G8_B8R8_2PLANE_422_UNORM,
        PixelFormatVideo_G8_B8_R8_3PLANE_444_UNORM,
        PixelFormatVideo_R10X6_UNORM_PACK16,
        PixelFormatVideo_R10X6G10X6_UNORM_2PACK16,
        PixelFormatVideo_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
        PixelFormatVideo_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
        PixelFormatVideo_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
        PixelFormatVideo_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
        PixelFormatVideo_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
        PixelFormatVideo_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
        PixelFormatVideo_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
        PixelFormatVideo_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
        PixelFormatVideo_R12X4_UNORM_PACK16,
        PixelFormatVideo_R12X4G12X4_UNORM_2PACK16,
        PixelFormatVideo_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
        PixelFormatVideo_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
        PixelFormatVideo_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
        PixelFormatVideo_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
        PixelFormatVideo_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
        PixelFormatVideo_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
        PixelFormatVideo_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
        PixelFormatVideo_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
        PixelFormatVideo_G16B16G16R16_422_UNORM,
        PixelFormatVideo_B16G16R16G16_422_UNORM,
        PixelFormatVideo_G16_B16_R16_3PLANE_420_UNORM,
        PixelFormatVideo_G16_B16R16_2PLANE_420_UNORM,
        PixelFormatVideo_G16_B16_R16_3PLANE_422_UNORM,
        PixelFormatVideo_G16_B16R16_2PLANE_422_UNORM,
        PixelFormatVideo_G16_B16_R16_3PLANE_444_UNORM,
        PixelFormatVideo_G8_B8R8_2PLANE_444_UNORM,
        PixelFormatVideo_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
        PixelFormatVideo_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
        PixelFormatVideo_G16_B16R16_2PLANE_444_UNORM,
        // Missing DX12 formats
        // DXGI_FORMAT_R11G11B10_FLOAT,
        // DXGI_FORMAT_R9G9B9E5_SHAREDEXP,
        // DXGI_FORMAT_R8G8_B8G8_UNORM,
        // DXGI_FORMAT_G8R8_G8B8_UNORM,
        // DXGI_FORMAT_B8G8R8X8_UNORM,
        // DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
        // DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
        // DXGI_FORMAT_AYUV -> DXGI_FORMAT_V408
        PixelFormat_MAX
    };

    enum EVertexFormat
    {
        VertexFormat_Unknown,
        VertexFormat_UINT_8X2,
        VertexFormat_UINT_8X4,
        VertexFormat_SINT_8X2,
        VertexFormat_SINT_8X4,
        VertexFormat_UNORM_8X2,
        VertexFormat_UNORM_8X4,
        VertexFormat_SNORM_8X2,
        VertexFormat_SNORM_8X4,
        VertexFormat_UINT_16X2,
        VertexFormat_UINT_16X4,
        VertexFormat_SINT_16X2,
        VertexFormat_SINT_16X4,
        VertexFormat_UNORM_16X2,
        VertexFormat_UNORM_16X4,
        VertexFormat_SNORM_16X2,
        VertexFormat_SNORM_16X4,
        VertexFormat_FLOAT_16X2,
        VertexFormat_FLOAT_16X4,
        VertexFormat_FLOAT_32X1,
        VertexFormat_FLOAT_32X2,
        VertexFormat_FLOAT_32X3,
        VertexFormat_FLOAT_32X4,
        VertexFormat_UINT_32X1,
        VertexFormat_UINT_32X2,
        VertexFormat_UINT_32X3,
        VertexFormat_UINT_32X4,
        VertexFormat_SINT_32X1,
        VertexFormat_SINT_32X2,
        VertexFormat_SINT_32X3,
        VertexFormat_SINT_32X4
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
        FaceWinding_CounterClockwise
    };

    enum EFilterMode
    {
        FilterMode_MinMagNearest,
        FilterMode_MinMagLinear,
        FilterMode_MinLinearMagNearest,
        FilterMode_MinNearestMagLinear,
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