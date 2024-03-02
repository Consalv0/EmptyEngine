
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
		{ L"PixelFormat_RG8",               2,      2,        true,    PixelFormat_RG8               },
		{ L"PixelFormat_RG32F",             8,      2,        true,    PixelFormat_RG32F             },
		{ L"PixelFormat_RG16F",             4,      2,        true,    PixelFormat_RG16F             },
		{ L"PixelFormat_RGB8",              3,      3,        true,    PixelFormat_RGB8              },
		{ L"PixelFormat_RGB32F",            12,     3,        true,    PixelFormat_RGB32F            },
		{ L"PixelFormat_RGB16F",            8,      3,        true,    PixelFormat_RGB16F            },
        { L"PixelFormat_RGBA8",             4,      4,        true,    PixelFormat_RGBA8             },
		{ L"PixelFormat_RGBA16_UShort",     8,      4,        true,    PixelFormat_RGBA16_UShort     },
		{ L"PixelFormat_RGBA32F",           16,     4,        true,    PixelFormat_RGBA32F           },
		{ L"PixelFormat_DepthComponent24",  4,      1,        true,    PixelFormat_DepthComponent24  },
		{ L"PixelFormat_DepthStencil",      4,      1,        true,    PixelFormat_DepthStencil      },
		{ L"PixelFormat_ShadowDepth",       4,      1,        true,    PixelFormat_ShadowDepth       }
	};

	PixelMap::PixelMap()
		: Width( 0 ), Height( 0 ), Depth( 0 ), pixelFormat( PixelFormat_Unknown ), data( NULL )
	{
	}

	PixelMap::PixelMap( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat )
		: Width( Width ), Height( Height ), Depth( Depth ), pixelFormat( pixelFormat ), data( NULL )
	{
		PixelMapUtility::CreateData( Width, Height, Depth, pixelFormat, data );
	}

	PixelMap::PixelMap( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat, void*& InData )
		: Width( Width ), Height( Height ), Depth( Depth ), pixelFormat( pixelFormat ), data( NULL )
	{
		PixelMapUtility::CreateData( Width, Height, Depth, pixelFormat, data, InData );
	}

	PixelMap::PixelMap( const PixelMap& Other )
		: Width( Other.Width ), Height( Other.Height ), Depth( Other.Depth ), pixelFormat( Other.pixelFormat ), data( NULL )
	{
		PixelMapUtility::CreateData( Width, Height, Depth, pixelFormat, data, Other.data );
	}

	void PixelMap::SetData( int32 InWidth, int32 InHeight, int32 InDepth, EPixelFormat InPixelFormat, void*& InData )
	{
		if ( data )
		{
			delete[] data;
			data = NULL;
		}
		Width = InWidth, Height = InHeight; Depth = InDepth;
		pixelFormat = InPixelFormat;
		PixelMapUtility::CreateData( Width, Height, Depth, pixelFormat, data, InData );
	}

	size_t PixelMap::GetMemorySize() const
	{
		return Width * Height * Depth * PixelFormats[ pixelFormat ].size_;
	}

	PixelMap& PixelMap::operator=( const PixelMap& Other )
	{
		if ( data )
		{
			delete[] data;
			data = NULL;
		}
		Width = Other.Width, Height = Other.Height; Depth = Other.Depth;
		pixelFormat = Other.pixelFormat;
		PixelMapUtility::CreateData( Width, Height, Depth, pixelFormat, data, Other.data );
		return *this;
	}

	PixelMap::~PixelMap()
	{
		delete[] data;
	}

	void PixelMapUtility::CreateData( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat, void*& data )
	{
		if ( data != NULL ) return;
		const size_t Size = Width * Height * Depth * (size_t)PixelFormats[ pixelFormat ].size_;
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

	void PixelMapUtility::CreateData( int32 Width, int32 Height, int32 Depth, EPixelFormat pixelFormat, void*& Target, void* data )
	{
		CreateData( Width, Height, Depth, pixelFormat, Target );
		if ( Target == NULL || data == NULL ) return;
		memcpy( Target, data, Width * Height * Depth * (size_t)PixelFormats[ pixelFormat ].size_ );
	}

	void PixelMapUtility::FlipVertically( PixelMap& Map )
	{
		switch ( Map.pixelFormat )
		{
		case PixelFormat_R8:      _FlipVertically< UCharRed>( Map.Width, Map.Height, Map.Depth, Map.data ); break;
		case PixelFormat_R32F:    _FlipVertically< FloatRed>( Map.Width, Map.Height, Map.Depth, Map.data ); break;
		case PixelFormat_RG8:     _FlipVertically<  UCharRG>( Map.Width, Map.Height, Map.Depth, Map.data ); break;
		case PixelFormat_RG32F:   _FlipVertically<  FloatRG>( Map.Width, Map.Height, Map.Depth, Map.data ); break;
		case PixelFormat_RGB8:    _FlipVertically< UCharRGB>( Map.Width, Map.Height, Map.Depth, Map.data ); break;
		case PixelFormat_RGB32F:  _FlipVertically< FloatRGB>( Map.Width, Map.Height, Map.Depth, Map.data ); break;
		case PixelFormat_RGBA8:   _FlipVertically<UCharRGBA>( Map.Width, Map.Height, Map.Depth, Map.data ); break;
		case PixelFormat_RGBA32F: _FlipVertically<FloatRGBA>( Map.Width, Map.Height, Map.Depth, Map.data ); break;
		}
	}

	unsigned char* PixelMapUtility::GetCharPixelAt( PixelMap& Map, const uint32& X, const uint32& Y, const uint32& Z )
	{
		const int32 Channels = PixelFormats[ Map.pixelFormat ].channels;
		return &((unsigned char*)Map.data)[
			Z * Map.Width * Map.Height * Channels +
				Y * Map.Width * Channels +
				X * Channels
		];
	}

	float* PixelMapUtility::GetFloatPixelAt( PixelMap& Map, const uint32& X, const uint32& Y, const uint32& Z )
	{
		const int32 Channels = PixelFormats[ Map.pixelFormat ].channels;
		return &((float*)Map.data)[
			Z * Map.Width * Map.Height * Channels +
				Y * Map.Width * Channels +
				X * Channels
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
		for ( uint32 Z = 0; Z < Map.Depth; ++Z )
		{
			for ( uint32 Y = 0; Y < Map.Height; ++Y )
			{
				for ( uint32 X = 0; X < Map.Width; ++X )
				{
					Function( GetCharPixelAt( Map, X, Y, Z ), Channels );
				}
			}
		}
	}

	void PixelMapUtility::PerPixelOperator( PixelMap& Map, std::function<void( float*, const unsigned char& )> const& Function )
	{
		const unsigned char& Channels = (unsigned char)PixelFormats[ Map.pixelFormat ].channels;
		for ( uint32 Z = 0; Z < Map.Depth; ++Z )
		{
			for ( uint32 Y = 0; Y < Map.Height; ++Y )
			{
				for ( uint32 X = 0; X < Map.Width; ++X )
				{
					Function( GetFloatPixelAt( Map, X, Y, Z ), Channels );
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