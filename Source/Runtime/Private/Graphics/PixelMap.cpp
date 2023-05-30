
#include "CoreMinimal.h"
#include "Graphics/Graphics.h"
#include "Graphics/PixelMap.h"

namespace EE
{
	constexpr EE::PixelFormatInfo PixelFormats[ EPixelFormat::Private_Num + 1 ] = {
		// Name                   Size  Channels  Supported  EPixelFormat
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
		: Width( 0 ), Height( 0 ), Depth( 0 ), PixelFormat( PixelFormat_Unknown ), Data( NULL )
	{
	}

	PixelMap::PixelMap( int Width, int Height, int Depth, EPixelFormat PixelFormat )
		: Width( Width ), Height( Height ), Depth( Depth ), PixelFormat( PixelFormat ), Data( NULL )
	{
		PixelMapUtility::CreateData( Width, Height, Depth, PixelFormat, Data );
	}

	PixelMap::PixelMap( int Width, int Height, int Depth, EPixelFormat PixelFormat, void*& InData )
		: Width( Width ), Height( Height ), Depth( Depth ), PixelFormat( PixelFormat ), Data( NULL )
	{
		PixelMapUtility::CreateData( Width, Height, Depth, PixelFormat, Data, InData );
	}

	PixelMap::PixelMap( const PixelMap& Other )
		: Width( Other.Width ), Height( Other.Height ), Depth( Other.Depth ), PixelFormat( Other.PixelFormat ), Data( NULL )
	{
		PixelMapUtility::CreateData( Width, Height, Depth, PixelFormat, Data, Other.Data );
	}

	void PixelMap::SetData( int InWidth, int InHeight, int InDepth, EPixelFormat InPixelFormat, void*& InData )
	{
		if ( Data )
		{
			delete[] Data;
			Data = NULL;
		}
		Width = InWidth, Height = InHeight; Depth = InDepth;
		PixelFormat = InPixelFormat;
		PixelMapUtility::CreateData( Width, Height, Depth, PixelFormat, Data, InData );
	}

	size_t PixelMap::GetMemorySize() const
	{
		return Width * Height * Depth * PixelFormats[ PixelFormat ].size_;
	}

	PixelMap& PixelMap::operator=( const PixelMap& Other )
	{
		if ( Data )
		{
			delete[] Data;
			Data = NULL;
		}
		Width = Other.Width, Height = Other.Height; Depth = Other.Depth;
		PixelFormat = Other.PixelFormat;
		PixelMapUtility::CreateData( Width, Height, Depth, PixelFormat, Data, Other.Data );
		return *this;
	}

	const void* PixelMap::PointerToValue() const
	{
		return Data;
	}

	PixelMap::~PixelMap()
	{
		delete[] Data;
	}

	void PixelMapUtility::CreateData( int Width, int Height, int Depth, EPixelFormat PixelFormat, void*& Data )
	{
		if ( Data != NULL ) return;
		const size_t Size = Width * Height * Depth * (size_t)PixelFormats[ PixelFormat ].size_;
		if ( Size == 0 )
		{
			Data = NULL;
			return;
		}
		if ( FormatIsFloat( PixelFormat ) )
			Data = new float[ Size ];
		else if ( FormatIsShort( PixelFormat ) )
			Data = new unsigned short[ Size ];
		else
			Data = new unsigned char[ Size ];
	}

	void PixelMapUtility::CreateData( int Width, int Height, int Depth, EPixelFormat PixelFormat, void*& Target, void* Data )
	{
		CreateData( Width, Height, Depth, PixelFormat, Target );
		if ( Target == NULL || Data == NULL ) return;
		memcpy( Target, Data, Width * Height * Depth * (size_t)PixelFormats[ PixelFormat ].size_ );
	}

	void PixelMapUtility::FlipVertically( PixelMap& Map )
	{
		switch ( Map.PixelFormat )
		{
		case PixelFormat_R8:      _FlipVertically< UCharRed>( Map.Width, Map.Height, Map.Depth, Map.Data ); break;
		case PixelFormat_R32F:    _FlipVertically< FloatRed>( Map.Width, Map.Height, Map.Depth, Map.Data ); break;
		case PixelFormat_RG8:     _FlipVertically<  UCharRG>( Map.Width, Map.Height, Map.Depth, Map.Data ); break;
		case PixelFormat_RG32F:   _FlipVertically<  FloatRG>( Map.Width, Map.Height, Map.Depth, Map.Data ); break;
		case PixelFormat_RGB8:    _FlipVertically< UCharRGB>( Map.Width, Map.Height, Map.Depth, Map.Data ); break;
		case PixelFormat_RGB32F:  _FlipVertically< FloatRGB>( Map.Width, Map.Height, Map.Depth, Map.Data ); break;
		case PixelFormat_RGBA8:   _FlipVertically<UCharRGBA>( Map.Width, Map.Height, Map.Depth, Map.Data ); break;
		case PixelFormat_RGBA32F: _FlipVertically<FloatRGBA>( Map.Width, Map.Height, Map.Depth, Map.Data ); break;
		}
	}

	unsigned char* PixelMapUtility::GetCharPixelAt( PixelMap& Map, const uint32_t& X, const uint32_t& Y, const uint32_t& Z )
	{
		const int Channels = PixelFormats[ Map.PixelFormat ].channels;
		return &((unsigned char*)Map.Data)[
			Z * Map.Width * Map.Height * Channels +
				Y * Map.Width * Channels +
				X * Channels
		];
	}

	float* PixelMapUtility::GetFloatPixelAt( PixelMap& Map, const uint32_t& X, const uint32_t& Y, const uint32_t& Z )
	{
		const int Channels = PixelFormats[ Map.PixelFormat ].channels;
		return &((float*)Map.Data)[
			Z * Map.Width * Map.Height * Channels +
				Y * Map.Width * Channels +
				X * Channels
		];
	}

	unsigned char* PixelMapUtility::GetCharPixelAt( PixelMap& Map, const size_t& Index )
	{
		return &((unsigned char*)Map.Data)[ Index * PixelFormats[ Map.PixelFormat ].channels ];
	}

	float* PixelMapUtility::GetFloatPixelAt( PixelMap& Map, const size_t& Index )
	{
		return &((float*)Map.Data)[ Index * PixelFormats[ Map.PixelFormat ].channels ];
	}

	void PixelMapUtility::PerPixelOperator( PixelMap& Map, std::function<void( unsigned char*, const unsigned char& )> const& Function )
	{
		const unsigned char& Channels = (unsigned char)PixelFormats[ Map.PixelFormat ].channels;
		for ( uint32_t Z = 0; Z < Map.Depth; ++Z )
		{
			for ( uint32_t Y = 0; Y < Map.Height; ++Y )
			{
				for ( uint32_t X = 0; X < Map.Width; ++X )
				{
					Function( GetCharPixelAt( Map, X, Y, Z ), Channels );
				}
			}
		}
	}

	void PixelMapUtility::PerPixelOperator( PixelMap& Map, std::function<void( float*, const unsigned char& )> const& Function )
	{
		const unsigned char& Channels = (unsigned char)PixelFormats[ Map.PixelFormat ].channels;
		for ( uint32_t Z = 0; Z < Map.Depth; ++Z )
		{
			for ( uint32_t Y = 0; Y < Map.Height; ++Y )
			{
				for ( uint32_t X = 0; X < Map.Width; ++X )
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