
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

namespace EE::Text
{
    NString ToNarrow(   const int8& number ) { return std::to_string( number ); }
    NString ToNarrow(  const int16& number ) { return std::to_string( number ); };
    NString ToNarrow(  const int32& number ) { return std::to_string( number ); };
    NString ToNarrow(  const int64& number ) { return std::to_string( number ); };
    NString ToNarrow(  const uint8& number ) { return std::to_string( number ); };
    NString ToNarrow( const uint16& number ) { return std::to_string( number ); };
    NString ToNarrow( const uint32& number ) { return std::to_string( number ); };
    NString ToNarrow( const uint64& number ) { return std::to_string( number ); };
    NString ToNarrow(  const float& number ) { return std::to_string( number ); };
    NString ToNarrow( const double& number ) { return std::to_string( number ); };

    WString ToWide(   const int8& number ) { return std::to_wstring( number ); };
    WString ToWide(  const int16& number ) { return std::to_wstring( number ); };
    WString ToWide(  const int32& number ) { return std::to_wstring( number ); };
    WString ToWide(  const int64& number ) { return std::to_wstring( number ); };
    WString ToWide(  const uint8& number ) { return std::to_wstring( number ); };
    WString ToWide( const uint16& number ) { return std::to_wstring( number ); };
    WString ToWide( const uint32& number ) { return std::to_wstring( number ); };
    WString ToWide( const uint64& number ) { return std::to_wstring( number ); };
    WString ToWide(  const float& number ) { return std::to_wstring( number ); };
    WString ToWide( const double& number ) { return std::to_wstring( number ); };

	NString WideToNarrow( const WChar* from )
	{
		if ( from == NULL )
			return "";

		std::mbstate_t state = std::mbstate_t();
		size_t sizeNeeded = std::wcsrtombs( NULL, &from, 0, &state );
		NString to = NString( sizeNeeded, '\0' );
		std::wcsrtombs( &to[ 0 ], &from, sizeNeeded, &state );

		return to;
	}

	WString NarrowToWide( const NChar* from )
	{
		if ( from == NULL )
			return L"";

		std::mbstate_t state = std::mbstate_t();
		size_t sizeNeeded = std::mbsrtowcs( NULL, &from, 0, &state );
		WString to = WString( sizeNeeded, L'\0' );
		std::mbsrtowcs( &to[ 0 ], &from, sizeNeeded, &state );

		return to;
	}

	NString WideToNarrow( const WString& from )
	{
		if ( from.empty() )
			return NString();

		size_t sizeNeeded = std::wcstombs( NULL, &from[ 0 ], 0 );
		NString to = NString( sizeNeeded, '\0' );
		to.resize( std::wcstombs( &to[ 0 ], from.c_str(), sizeNeeded ) );

		return to;
	}

	WString NarrowToWide( const NString& from )
	{
		if ( from.empty() )
			return WString();

		size_t sizeNeeded = std::mbstowcs( NULL, &from[ 0 ], 0 );
		WString to = WString( sizeNeeded, '\0' );
		to.resize( std::mbstowcs( &to[ 0 ], from.c_str(), sizeNeeded ) );

		return to;
	}
}
