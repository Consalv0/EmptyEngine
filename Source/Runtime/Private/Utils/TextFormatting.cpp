
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

namespace EE::Text
{
	NString WideToNarrow( const WChar* From )
	{
		if ( From == NULL )
			return "";

		std::mbstate_t state = std::mbstate_t();
		size_t SizeNeeded = std::wcsrtombs( NULL, &From, 0, &state );
		NString To = NString( SizeNeeded, '\0' );
		std::wcsrtombs( &To[ 0 ], &From, SizeNeeded, &state );

		return To;
	}

	WString NarrowToWide( const NChar* From )
	{
		if ( From == NULL )
			return L"";

		std::mbstate_t state = std::mbstate_t();
		size_t SizeNeeded = std::mbsrtowcs( NULL, &From, 0, &state );
		WString To = WString( SizeNeeded, L'\0' );
		std::mbsrtowcs( &To[ 0 ], &From, SizeNeeded, &state );

		return To;
	}

	NString WideToNarrow( const WString& From )
	{
		if ( From.empty() )
			return NString();

		size_t SizeNeeded = std::wcstombs( NULL, &From[ 0 ], 0 );
		NString To = NString( SizeNeeded, '\0' );
		To.resize( std::wcstombs( &To[ 0 ], From.c_str(), SizeNeeded ) );

		return To;
	}

	WString NarrowToWide( const NString& From )
	{
		if ( From.empty() )
			return WString();

		size_t SizeNeeded = std::mbstowcs( NULL, &From[ 0 ], 0 );
		WString To = WString( SizeNeeded, '\0' );
		To.resize( std::mbstowcs( &To[ 0 ], From.c_str(), SizeNeeded ) );

		return To;
	}
}
