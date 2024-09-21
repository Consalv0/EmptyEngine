
#include "CoreMinimal.h"

#include "Math/CoreMath.h"
#include "Rendering/Common.h"
#include "Rendering/PixelMap.h"

namespace EE
{
    PixelFormatInfo GPixelFormatInfo[ EPixelFormat::PixelFormat_MAX ] =
    {
        // Name                              Size   Channels  Supported    EPixelFormat
        { L"Unknown",                         0,       0,       false,      PixelFormat_Unknown },
        { L"R4G4_UNORM_PACK8",                0,       2,       false,      PixelFormat_R4G4_UNORM_PACK8 },
        { L"R4G4B4A4_UNORM_PACK16",           0,       4,       false,      PixelFormat_R4G4B4A4_UNORM_PACK16 },
        { L"B4G4R4A4_UNORM_PACK16",           0,       4,       false,      PixelFormat_B4G4R4A4_UNORM_PACK16 },
        { L"R5G6B5_UNORM_PACK16",             0,       3,       false,      PixelFormat_R5G6B5_UNORM_PACK16 },
        { L"B5G6R5_UNORM_PACK16",             0,       3,       false,      PixelFormat_B5G6R5_UNORM_PACK16 },  
        { L"R5G5B5A1_UNORM_PACK16",           0,       4,       false,      PixelFormat_R5G5B5A1_UNORM_PACK16 },
        { L"B5G5R5A1_UNORM_PACK16",           0,       4,       false,      PixelFormat_B5G5R5A1_UNORM_PACK16 },
        { L"A1R5G5B5_UNORM_PACK16",           0,       4,       false,      PixelFormat_A1R5G5B5_UNORM_PACK16 },
        { L"R8_UNORM",                        1,       1,       false,      PixelFormat_R8_UNORM },
        { L"R8_SNORM",                        1,       1,       false,      PixelFormat_R8_SNORM },
        { L"R8_USCALED",                      1,       1,       false,      PixelFormat_R8_USCALED },
        { L"R8_SSCALED",                      1,       1,       false,      PixelFormat_R8_SSCALED },
        { L"R8_UINT",                         1,       1,       false,      PixelFormat_R8_UINT },
        { L"R8_SINT",                         1,       1,       false,      PixelFormat_R8_SINT },
        { L"R8_SRGB",                         1,       1,       false,      PixelFormat_R8_SRGB },
        { L"R8G8_UNORM",                      1,       2,       false,      PixelFormat_R8G8_UNORM },
        { L"R8G8_SNORM",                      1,       2,       false,      PixelFormat_R8G8_SNORM },
        { L"R8G8_USCALED",                    1,       2,       false,      PixelFormat_R8G8_USCALED },
        { L"R8G8_SSCALED",                    1,       2,       false,      PixelFormat_R8G8_SSCALED },
        { L"R8G8_UINT",                       1,       2,       false,      PixelFormat_R8G8_UINT },
        { L"R8G8_SINT",                       1,       2,       false,      PixelFormat_R8G8_SINT },
        { L"R8G8_SRGB",                       1,       2,       false,      PixelFormat_R8G8_SRGB },
        { L"R8G8B8_UNORM",                    1,       3,       false,      PixelFormat_R8G8B8_UNORM },
        { L"R8G8B8_SNORM",                    1,       3,       false,      PixelFormat_R8G8B8_SNORM },
        { L"R8G8B8_USCALED",                  1,       3,       false,      PixelFormat_R8G8B8_USCALED },
        { L"R8G8B8_SSCALED",                  1,       3,       false,      PixelFormat_R8G8B8_SSCALED },
        { L"R8G8B8_UINT",                     1,       3,       false,      PixelFormat_R8G8B8_UINT },
        { L"R8G8B8_SINT",                     1,       3,       false,      PixelFormat_R8G8B8_SINT },
        { L"R8G8B8_SRGB",                     1,       3,       false,      PixelFormat_R8G8B8_SRGB },
        { L"B8G8R8_UNORM",                    1,       3,       false,      PixelFormat_B8G8R8_UNORM },
        { L"B8G8R8_SNORM",                    1,       3,       false,      PixelFormat_B8G8R8_SNORM },
        { L"B8G8R8_USCALED",                  1,       3,       false,      PixelFormat_B8G8R8_USCALED },
        { L"B8G8R8_SSCALED",                  1,       3,       false,      PixelFormat_B8G8R8_SSCALED },
        { L"B8G8R8_UINT",                     1,       3,       false,      PixelFormat_B8G8R8_UINT },
        { L"B8G8R8_SINT",                     1,       3,       false,      PixelFormat_B8G8R8_SINT },
        { L"B8G8R8_SRGB",                     1,       3,       false,      PixelFormat_B8G8R8_SRGB },
        { L"R8G8B8A8_UNORM",                  1,       4,       true,       PixelFormat_R8G8B8A8_UNORM },
        { L"R8G8B8A8_SNORM",                  1,       4,       true,       PixelFormat_R8G8B8A8_SNORM },
        { L"R8G8B8A8_USCALED",                1,       4,       true,       PixelFormat_R8G8B8A8_USCALED },
        { L"R8G8B8A8_SSCALED",                1,       4,       true,       PixelFormat_R8G8B8A8_SSCALED },
        { L"R8G8B8A8_UINT",                   1,       4,       true,       PixelFormat_R8G8B8A8_UINT },
        { L"R8G8B8A8_SINT",                   1,       4,       true,       PixelFormat_R8G8B8A8_SINT },
        { L"R8G8B8A8_SRGB",                   1,       4,       true,       PixelFormat_R8G8B8A8_SRGB },
        { L"B8G8R8A8_UNORM",                  1,       4,       true,       PixelFormat_B8G8R8A8_UNORM },
        { L"B8G8R8A8_SNORM",                  1,       4,       true,       PixelFormat_B8G8R8A8_SNORM },
        { L"B8G8R8A8_USCALED",                1,       4,       true,       PixelFormat_B8G8R8A8_USCALED },
        { L"B8G8R8A8_SSCALED",                1,       4,       true,       PixelFormat_B8G8R8A8_SSCALED },
        { L"B8G8R8A8_UINT",                   1,       4,       true,       PixelFormat_B8G8R8A8_UINT },
        { L"B8G8R8A8_SINT",                   1,       4,       true,       PixelFormat_B8G8R8A8_SINT },
        { L"B8G8R8A8_SRGB",                   1,       4,       true,       PixelFormat_B8G8R8A8_SRGB },
        { L"A8B8G8R8_UNORM_PACK32",           1,       4,       false,      PixelFormat_A8B8G8R8_UNORM_PACK32 },
        { L"A8B8G8R8_SNORM_PACK32",           1,       4,       false,      PixelFormat_A8B8G8R8_SNORM_PACK32 },
        { L"A8B8G8R8_USCALED_PACK32",         1,       4,       false,      PixelFormat_A8B8G8R8_USCALED_PACK32 },
        { L"A8B8G8R8_SSCALED_PACK32",         1,       4,       false,      PixelFormat_A8B8G8R8_SSCALED_PACK32 },
        { L"A8B8G8R8_UINT_PACK32",            1,       4,       false,      PixelFormat_A8B8G8R8_UINT_PACK32 },
        { L"A8B8G8R8_SINT_PACK32",            1,       4,       false,      PixelFormat_A8B8G8R8_SINT_PACK32 },
        { L"A8B8G8R8_SRGB_PACK32",            1,       4,       false,      PixelFormat_A8B8G8R8_SRGB_PACK32 },
        { L"A2R10G10B10_UNORM_PACK32",        1,       1,       false,      PixelFormat_A2R10G10B10_UNORM_PACK32 },   
        { L"A2R10G10B10_SNORM_PACK32",        1,       1,       false,      PixelFormat_A2R10G10B10_SNORM_PACK32 },
        { L"A2R10G10B10_USCALED_PACK32",      1,       1,       false,      PixelFormat_A2R10G10B10_USCALED_PACK32 },
        { L"A2R10G10B10_SSCALED_PACK32",      1,       1,       false,      PixelFormat_A2R10G10B10_SSCALED_PACK32 },
        { L"A2R10G10B10_UINT_PACK32",         1,       1,       false,      PixelFormat_A2R10G10B10_UINT_PACK32 },    
        { L"A2R10G10B10_SINT_PACK32",         1,       1,       false,      PixelFormat_A2R10G10B10_SINT_PACK32 },
        { L"A2B10G10R10_UNORM_PACK32",        1,       1,       false,      PixelFormat_A2B10G10R10_UNORM_PACK32 },
        { L"A2B10G10R10_SNORM_PACK32",        1,       1,       false,      PixelFormat_A2B10G10R10_SNORM_PACK32 },
        { L"A2B10G10R10_USCALED_PACK32",      1,       1,       false,      PixelFormat_A2B10G10R10_USCALED_PACK32 },
        { L"A2B10G10R10_SSCALED_PACK32",      1,       1,       false,      PixelFormat_A2B10G10R10_SSCALED_PACK32 },
        { L"A2B10G10R10_UINT_PACK32",         1,       1,       false,      PixelFormat_A2B10G10R10_UINT_PACK32 },
        { L"A2B10G10R10_SINT_PACK32",         1,       1,       false,      PixelFormat_A2B10G10R10_SINT_PACK32 },
        { L"R16_UNORM",                       1,       1,       false,      PixelFormat_R16_UNORM },             
        { L"R16_SNORM",                       1,       1,       false,      PixelFormat_R16_SNORM },             
        { L"R16_USCALED",                     1,       1,       false,      PixelFormat_R16_USCALED },
        { L"R16_SSCALED",                     1,       1,       false,      PixelFormat_R16_SSCALED },
        { L"R16_UINT",                        1,       1,       false,      PixelFormat_R16_UINT },              
        { L"R16_SINT",                        1,       1,       false,      PixelFormat_R16_SINT },              
        { L"R16_SFLOAT",                      1,       1,       false,      PixelFormat_R16_SFLOAT },            
        { L"R16G16_UNORM",                    1,       1,       false,      PixelFormat_R16G16_UNORM },          
        { L"R16G16_SNORM",                    1,       1,       false,      PixelFormat_R16G16_SNORM },          
        { L"R16G16_USCALED",                  1,       1,       false,      PixelFormat_R16G16_USCALED },
        { L"R16G16_SSCALED",                  1,       1,       false,      PixelFormat_R16G16_SSCALED },
        { L"R16G16_UINT",                     1,       1,       false,      PixelFormat_R16G16_UINT },           
        { L"R16G16_SINT",                     1,       1,       false,      PixelFormat_R16G16_SINT },           
        { L"R16G16_SFLOAT",                   1,       1,       false,      PixelFormat_R16G16_SFLOAT },         
        { L"R16G16B16_UNORM",                 1,       1,       false,      PixelFormat_R16G16B16_UNORM },
        { L"R16G16B16_SNORM",                 1,       1,       false,      PixelFormat_R16G16B16_SNORM },
        { L"R16G16B16_USCALED",               1,       1,       false,      PixelFormat_R16G16B16_USCALED },
        { L"R16G16B16_SSCALED",               1,       1,       false,      PixelFormat_R16G16B16_SSCALED },
        { L"R16G16B16_UINT",                  1,       1,       false,      PixelFormat_R16G16B16_UINT },
        { L"R16G16B16_SINT",                  1,       1,       false,      PixelFormat_R16G16B16_SINT },
        { L"R16G16B16_SFLOAT",                1,       1,       false,      PixelFormat_R16G16B16_SFLOAT },
        { L"R16G16B16A16_UNORM",              1,       1,       false,      PixelFormat_R16G16B16A16_UNORM },    
        { L"R16G16B16A16_SNORM",              1,       1,       false,      PixelFormat_R16G16B16A16_SNORM },    
        { L"R16G16B16A16_USCALED",            1,       1,       false,      PixelFormat_R16G16B16A16_USCALED },
        { L"R16G16B16A16_SSCALED",            1,       1,       false,      PixelFormat_R16G16B16A16_SSCALED },
        { L"R16G16B16A16_UINT",               1,       1,       false,      PixelFormat_R16G16B16A16_UINT },     
        { L"R16G16B16A16_SINT",               1,       1,       false,      PixelFormat_R16G16B16A16_SINT },     
        { L"R16G16B16A16_SFLOAT",             1,       1,       false,      PixelFormat_R16G16B16A16_SFLOAT },   
        { L"R32_UINT",                        1,       1,       false,      PixelFormat_R32_UINT },              
        { L"R32_SINT",                        1,       1,       false,      PixelFormat_R32_SINT },              
        { L"R32_SFLOAT",                      1,       1,       false,      PixelFormat_R32_SFLOAT },            
        { L"R32G32_UINT",                     1,       1,       false,      PixelFormat_R32G32_UINT },           
        { L"R32G32_SINT",                     1,       1,       false,      PixelFormat_R32G32_SINT },           
        { L"R32G32_SFLOAT",                   1,       1,       false,      PixelFormat_R32G32_SFLOAT },         
        { L"R32G32B32_UINT",                  1,       1,       false,      PixelFormat_R32G32B32_UINT },        
        { L"R32G32B32_SINT",                  1,       1,       false,      PixelFormat_R32G32B32_SINT },        
        { L"R32G32B32_SFLOAT",                1,       1,       false,      PixelFormat_R32G32B32_SFLOAT },      
        { L"R32G32B32A32_UINT",               1,       1,       false,      PixelFormat_R32G32B32A32_UINT },     
        { L"R32G32B32A32_SINT",               1,       1,       false,      PixelFormat_R32G32B32A32_SINT },     
        { L"R32G32B32A32_SFLOAT",             1,       1,       false,      PixelFormat_R32G32B32A32_SFLOAT },   
        { L"R64_UINT",                        1,       1,       false,      PixelFormat_R64_UINT },
        { L"R64_SINT",                        1,       1,       false,      PixelFormat_R64_SINT },
        { L"R64_SFLOAT",                      1,       1,       false,      PixelFormat_R64_SFLOAT },
        { L"R64G64_UINT",                     1,       1,       false,      PixelFormat_R64G64_UINT },
        { L"R64G64_SINT",                     1,       1,       false,      PixelFormat_R64G64_SINT },
        { L"R64G64_SFLOAT",                   1,       1,       false,      PixelFormat_R64G64_SFLOAT },
        { L"R64G64B64_UINT",                  1,       1,       false,      PixelFormat_R64G64B64_UINT },
        { L"R64G64B64_SINT",                  1,       1,       false,      PixelFormat_R64G64B64_SINT },
        { L"R64G64B64_SFLOAT",                1,       1,       false,      PixelFormat_R64G64B64_SFLOAT },
        { L"R64G64B64A64_UINT",               1,       1,       false,      PixelFormat_R64G64B64A64_UINT },
        { L"R64G64B64A64_SINT",               1,       1,       false,      PixelFormat_R64G64B64A64_SINT },
        { L"R64G64B64A64_SFLOAT",             1,       1,       false,      PixelFormat_R64G64B64A64_SFLOAT },
    };
    
    PixelMap::PixelMap()
        : width_( 0 ), height_( 0 ), depth_( 0 ), pixelFormat_( PixelFormat_Unknown ), data_( NULL )
    {
    }

    PixelMap::PixelMap( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat )
        : width_( width ), height_( height ), depth_( depth ), pixelFormat_( pixelFormat ), data_( NULL )
    {
        PixelMapUtility::CreateData( width_, height_, depth_, pixelFormat_, &data_ );
    }

    PixelMap::PixelMap( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, const void* inData )
        : width_( width ), height_( height ), depth_( depth ), pixelFormat_( pixelFormat ), data_( NULL )
    {
        PixelMapUtility::CreateData( width_, height_, depth_, pixelFormat_, &data_, inData );
    }

    void PixelMap::Clear()
    {
        if ( data_ )
        {
            free( data_ );
            data_ = NULL;
        }
    }

    void PixelMap::Swap( PixelMap& other )
    {
        void* otherData = data_;
        data_ = other.data_;
        other.data_ = otherData;
        uint32 otherWidth = width_;
        width_ = other.width_;
        other.width_ = otherWidth;
        uint32 otherHeight = height_;
        height_ = other.height_;
        other.height_ = otherHeight;
        uint32 otherDepth = depth_;
        depth_ = other.depth_;
        other.depth_ = otherDepth;
        EPixelFormat otherPixelFormat = pixelFormat_;
        pixelFormat_ = other.pixelFormat_;
        other.pixelFormat_ = otherPixelFormat;
    }

    void PixelMap::SetData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, const void* data )
    {
        Clear();
        this->width_ = width, this->height_ = height; this->depth_ = depth;
        this->pixelFormat_ = pixelFormat;
        PixelMapUtility::CreateData( width, height, depth, pixelFormat, &data_, data );
    }

    size_t PixelMap::GetSize() const
    {
        return width_ * height_ * depth_ * GPixelFormatInfo[ pixelFormat_ ].size * GPixelFormatInfo[ pixelFormat_ ].channels;
    }

    PixelMap::~PixelMap()
    {
        Clear();
    }

    void PixelMapUtility::CreateData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void** data )
    {
        if ( *data != NULL ) return;
        const size_t size = (size_t)width * height * depth * GPixelFormatInfo[ pixelFormat ].size * GPixelFormatInfo[ pixelFormat ].channels;
        if ( size == 0 )
        {
            *data = NULL;
            return;
        }

        *data = malloc( size );
    }

    void PixelMapUtility::CreateData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void** target, const void* data )
    {
        CreateData( width, height, depth, pixelFormat, target );
        if ( *target == NULL || data == NULL ) return;
        memcpy( *target, data, (size_t)width * height * depth * GPixelFormatInfo[ pixelFormat ].size * GPixelFormatInfo[ pixelFormat ].channels );
    }

    void PixelMapUtility::FlipVertically( PixelMap& map )
    {
        switch ( map.pixelFormat_ )
        {
        case PixelFormat_R8_UINT:        FlipVertically< UCharRed>( map.width_, map.height_, map.depth_, map.data_ ); break;
        case PixelFormat_R32_UINT:       FlipVertically< UCharRed>( map.width_, map.height_, map.depth_, map.data_ ); break;
        case PixelFormat_R8G8_UINT:      FlipVertically<  UCharRG>( map.width_, map.height_, map.depth_, map.data_ ); break;
        case PixelFormat_R16G16_UNORM:   FlipVertically<  FloatRG>( map.width_, map.height_, map.depth_, map.data_ ); break;
        case PixelFormat_R8G8B8A8_UINT:  FlipVertically<UCharRGBA>( map.width_, map.height_, map.depth_, map.data_ ); break;
        case PixelFormat_R8G8B8A8_UNORM: FlipVertically<FloatRGBA>( map.width_, map.height_, map.depth_, map.data_ ); break;
        }
    }

    unsigned char* PixelMapUtility::GetCharPixelAt( PixelMap& map, const uint32& x, const uint32& y, const uint32& z )
    {
        const int32 channels = GPixelFormatInfo[ map.pixelFormat_ ].channels;
        return &((unsigned char*)map.data_)
            [
                z * map.width_ * map.height_ * channels +
                y * map.width_ * channels +
                x * channels
            ];
    }

    float* PixelMapUtility::GetFloatPixelAt( PixelMap& map, const uint32& x, const uint32& y, const uint32& z )
    {
        const int32 channels = GPixelFormatInfo[ map.pixelFormat_ ].channels;
        return &((float*)map.data_)
            [
                z * map.width_ * map.height_ * channels +
                y * map.width_ * channels +
                x * channels
            ];
    }

    unsigned char* PixelMapUtility::GetCharPixelAt( PixelMap& map, const size_t& index )
    {
        return &((unsigned char*)map.data_)[ index * GPixelFormatInfo[ map.pixelFormat_ ].channels ];
    }

    float* PixelMapUtility::GetFloatPixelAt( PixelMap& map, const size_t& index )
    {
        return &((float*)map.data_)[ index * GPixelFormatInfo[ map.pixelFormat_ ].channels ];
    }

    void PixelMapUtility::PerPixelOperator( PixelMap& map, std::function<void( unsigned char*, const unsigned char& )> const& function )
    {
        const unsigned char& channels = (unsigned char)GPixelFormatInfo[ map.pixelFormat_ ].channels;
        for ( uint32 Z = 0; Z < map.depth_; ++Z )
        {
            for ( uint32 Y = 0; Y < map.height_; ++Y )
            {
                for ( uint32 X = 0; X < map.width_; ++X )
                {
                    function( GetCharPixelAt( map, X, Y, Z ), channels );
                }
            }
        }
    }

    void PixelMapUtility::PerPixelOperator( PixelMap& map, std::function<void( float*, const unsigned char& )> const& function )
    {
        const unsigned char& channels = (unsigned char)GPixelFormatInfo[ map.pixelFormat_ ].channels;
        for ( uint32 Z = 0; Z < map.depth_; ++Z )
        {
            for ( uint32 Y = 0; Y < map.height_; ++Y )
            {
                for ( uint32 X = 0; X < map.width_; ++X )
                {
                    function( GetFloatPixelAt( map, X, Y, Z ), channels );
                }
            }
        }
    }
}