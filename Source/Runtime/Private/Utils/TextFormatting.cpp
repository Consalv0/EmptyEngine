
#include "CoreMinimal.h"
#include "Utils/TextFormatting.h"

namespace EE::Text
{
    U8String ToUTF8(   const int8& number ) { return std::to_string( number ); }
    U8String ToUTF8(  const int16& number ) { return std::to_string( number ); };
    U8String ToUTF8(  const int32& number ) { return std::to_string( number ); };
    U8String ToUTF8(  const int64& number ) { return std::to_string( number ); };
    U8String ToUTF8(  const uint8& number ) { return std::to_string( number ); };
    U8String ToUTF8( const uint16& number ) { return std::to_string( number ); };
    U8String ToUTF8( const uint32& number ) { return std::to_string( number ); };
    U8String ToUTF8( const uint64& number ) { return std::to_string( number ); };
    U8String ToUTF8(  const float& number ) { return std::to_string( number ); };
    U8String ToUTF8( const double& number ) { return std::to_string( number ); };

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

    U8String WideToUTF8( const WChar* from )
    {
        if ( from == NULL )
            return "";

        std::mbstate_t state = std::mbstate_t();
        size_t sizeNeeded = std::wcsrtombs( NULL, &from, 0, &state );
        U8String to = U8String( sizeNeeded, '\0' );
        std::wcsrtombs( &to[ 0 ], &from, sizeNeeded, &state );

        return to;
    }

    U8String WideToUTF8( const WString& from )
    {
        if ( from.empty() )
            return "";

        size_t sizeNeeded = std::wcstombs( NULL, &from[ 0 ], 0 );
        U8String to = U8String( sizeNeeded, '\0' );
        to.resize( std::wcstombs( &to[ 0 ], from.c_str(), sizeNeeded ) );

        return to;
    }

    WString UTF8ToWide( const U8Char* from )
    {
        if ( from == NULL )
            return WString();

        std::mbstate_t state = std::mbstate_t();
        size_t sizeNeeded = std::mbsrtowcs( NULL, &from, 0, &state );
        WString to = WString( sizeNeeded, L'\0' );
        std::mbsrtowcs( &to[ 0 ], &from, sizeNeeded, &state );

        return to;
    }

    WString UTF8ToWide( const U8String& from )
    {
        if ( from.empty() )
            return WString();

        size_t sizeNeeded = std::mbstowcs( NULL, &from[ 0 ], 0 );
        WString to = WString( sizeNeeded, '\0' );
        to.resize( std::mbstowcs( &to[ 0 ], from.c_str(), sizeNeeded ) );

        return to;
    }
}
