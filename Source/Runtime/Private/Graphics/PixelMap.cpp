
#include "CoreMinimal.h"
#include "Graphics/Graphics.h"
#include "Graphics/PixelMap.h"

namespace EE
{
    constexpr EE::PixelFormatInfo PixelFormats[ EPixelFormat::Private_Num + 1 ] = {
        // Name                           Size   Channels  Supported   EPixelFormat
        { L"PixelFormat_Unknown",           0,      0,       false,    PixelFormat_Unknown           },
        { L"PixelFormat_R8",                1,      1,        true,    PixelFormat_R8                },
        { L"PixelFormat_R32F",              4,      1,        true,    PixelFormat_R32F              },
        { L"PixelFormat_R8G8",               2,      2,        true,    PixelFormat_R8G8               },
        { L"PixelFormat_RG32F",             8,      2,        true,    PixelFormat_RG32F             },
        { L"PixelFormat_RG16F",             4,      2,        true,    PixelFormat_RG16F             },
        { L"PixelFormat_R8G8B8",              3,      3,        true,    PixelFormat_R8G8B8              },
        { L"PixelFormat_RGB32F",            12,     3,        true,    PixelFormat_RGB32F            },
        { L"PixelFormat_RGB16F",            8,      3,        true,    PixelFormat_RGB16F            },
        { L"PixelFormat_R8G8B8A8",             4,      4,        true,    PixelFormat_R8G8B8A8             },
        { L"PixelFormat_RGBA16_UShort",     8,      4,        true,    PixelFormat_RGBA16_UShort     },
        { L"PixelFormat_RGBA32F",           16,     4,        true,    PixelFormat_RGBA32F           },
        { L"PixelFormat_DepthComponent24",  4,      1,        true,    PixelFormat_DepthComponent24  },
        { L"PixelFormat_DepthStencil",      4,      1,        true,    PixelFormat_DepthStencil      },
        { L"PixelFormat_ShadowDepth",       4,      1,        true,    PixelFormat_ShadowDepth       }
    };

    PixelMap::PixelMap()
        : width( 0 ), height( 0 ), depth( 0 ), pixelFormat( PixelFormat_Unknown ), data( NULL )
    {
    }

    PixelMap::PixelMap( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat )
        : width( width ), height( height ), depth( depth ), pixelFormat( pixelFormat ), data( NULL )
    {
        PixelMapUtility::CreateData( width, height, depth, pixelFormat, data );
    }

    PixelMap::PixelMap( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void*& inData )
        : width( width ), height( height ), depth( depth ), pixelFormat( pixelFormat ), data( NULL )
    {
        PixelMapUtility::CreateData( width, height, depth, pixelFormat, data, inData );
    }

    PixelMap::PixelMap( const PixelMap& other )
        : width( other.width ), height( other.height ), depth( other.depth ), pixelFormat( other.pixelFormat ), data( NULL )
    {
        PixelMapUtility::CreateData( width, height, depth, pixelFormat, data, other.data );
    }

    void PixelMap::SetData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void*& data )
    {
        if ( data )
        {
            delete[] data;
            data = NULL;
        }
        this->width = width, this->height = height; this->depth = depth;
        this->pixelFormat = pixelFormat;
        PixelMapUtility::CreateData( width, height, depth, pixelFormat, data, data );
    }

    size_t PixelMap::GetMemorySize() const
    {
        return width * height * depth * PixelFormats[ pixelFormat ].size;
    }

    PixelMap& PixelMap::operator=( const PixelMap& other )
    {
        if ( data )
        {
            delete[] data;
            data = NULL;
        }
        width = other.width, height = other.height; depth = other.depth;
        pixelFormat = other.pixelFormat;
        PixelMapUtility::CreateData( width, height, depth, pixelFormat, data, other.data );
        return *this;
    }

    PixelMap::~PixelMap()
    {
        delete[] data;
    }

    void PixelMapUtility::CreateData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void*& data )
    {
        if ( data != NULL ) return;
        const size_t Size = width * height * depth * (size_t)PixelFormats[ pixelFormat ].size;
        if ( Size == 0 )
        {
            data = NULL;
            return;
        }
        if ( FormatIsFloat( pixelFormat ) )
            data = new float[ Size ];
        else if ( FormatIsShort( pixelFormat ) )
            data = new unsigned short[ Size ];
        else
            data = new unsigned char[ Size ];
    }

    void PixelMapUtility::CreateData( int32 width, int32 height, int32 depth, EPixelFormat pixelFormat, void*& target, void* data )
    {
        CreateData( width, height, depth, pixelFormat, target );
        if ( target == NULL || data == NULL ) return;
        memcpy( target, data, width * height * depth * (size_t)PixelFormats[ pixelFormat ].size );
    }

    void PixelMapUtility::FlipVertically( PixelMap& map )
    {
        switch ( map.pixelFormat )
        {
        case PixelFormat_R8:      _FlipVertically< UCharRed>( map.width, map.height, map.depth, map.data ); break;
        case PixelFormat_R32F:    _FlipVertically< FloatRed>( map.width, map.height, map.depth, map.data ); break;
        case PixelFormat_R8G8:     _FlipVertically<  UCharRG>( map.width, map.height, map.depth, map.data ); break;
        case PixelFormat_RG32F:   _FlipVertically<  FloatRG>( map.width, map.height, map.depth, map.data ); break;
        case PixelFormat_R8G8B8:    _FlipVertically< UCharRGB>( map.width, map.height, map.depth, map.data ); break;
        case PixelFormat_RGB32F:  _FlipVertically< FloatRGB>( map.width, map.height, map.depth, map.data ); break;
        case PixelFormat_R8G8B8A8:   _FlipVertically<UCharRGBA>( map.width, map.height, map.depth, map.data ); break;
        case PixelFormat_RGBA32F: _FlipVertically<FloatRGBA>( map.width, map.height, map.depth, map.data ); break;
        }
    }

    unsigned char* PixelMapUtility::GetCharPixelAt( PixelMap& map, const uint32& x, const uint32& y, const uint32& z )
    {
        const int32 channels = PixelFormats[ map.pixelFormat ].channels;
        return &((unsigned char*)map.data)
            [
                z * map.width * map.height * channels +
                y * map.width * channels +
                x * channels
            ];
    }

    float* PixelMapUtility::GetFloatPixelAt( PixelMap& map, const uint32& x, const uint32& y, const uint32& z )
    {
        const int32 channels = PixelFormats[ map.pixelFormat ].channels;
        return &((float*)map.data)
            [
                z * map.width * map.height * channels +
                y * map.width * channels +
                x * channels
            ];
    }

    unsigned char* PixelMapUtility::GetCharPixelAt( PixelMap& Map, const size_t& Index )
    {
        return &((unsigned char*)Map.data)[ Index * PixelFormats[ Map.pixelFormat ].channels ];
    }

    float* PixelMapUtility::GetFloatPixelAt( PixelMap& Map, const size_t& Index )
    {
        return &((float*)Map.data)[ Index * PixelFormats[ Map.pixelFormat ].channels ];
    }

    void PixelMapUtility::PerPixelOperator( PixelMap& Map, std::function<void( unsigned char*, const unsigned char& )> const& Function )
    {
        const unsigned char& Channels = (unsigned char)PixelFormats[ Map.pixelFormat ].channels;
        for ( uint32 Z = 0; Z < Map.depth; ++Z )
        {
            for ( uint32 Y = 0; Y < Map.height; ++Y )
            {
                for ( uint32 X = 0; X < Map.width; ++X )
                {
                    Function( GetCharPixelAt( Map, X, Y, Z ), Channels );
                }
            }
        }
    }

    void PixelMapUtility::PerPixelOperator( PixelMap& map, std::function<void( float*, const unsigned char& )> const& function )
    {
        const unsigned char& channels = (unsigned char)PixelFormats[ map.pixelFormat ].channels;
        for ( uint32 Z = 0; Z < map.depth; ++Z )
        {
            for ( uint32 Y = 0; Y < map.height; ++Y )
            {
                for ( uint32 X = 0; X < map.width; ++X )
                {
                    function( GetFloatPixelAt( map, X, Y, Z ), channels );
                }
            }
        }
    }

    bool PixelMapUtility::FormatIsFloat( EPixelFormat format )
    {
        switch ( format )
        {
        case PixelFormat_R32F:
        case PixelFormat_RG32F:
        case PixelFormat_RG16F:
        case PixelFormat_RGB32F:
        case PixelFormat_RGBA32F:
        case PixelFormat_DepthStencil:
        case PixelFormat_ShadowDepth:
            return true;
        default:
            return false;
        }
    }

    bool PixelMapUtility::FormatIsShort( EPixelFormat format )
    {
        switch ( format )
        {
        case PixelFormat_RGBA16_UShort:
            return true;
        default:
            return false;
        }
    }
}