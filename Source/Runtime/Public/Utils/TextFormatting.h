#pragma once

#include "CoreTypes.h"
#include <algorithm>

namespace EE
{
    namespace Text
    {
        U8String ToUTF8( const int8& number );
        U8String ToUTF8( const int16& number );
        U8String ToUTF8( const int32& number );
        U8String ToUTF8( const int64& number );
        U8String ToUTF8( const uint8& number );
        U8String ToUTF8( const uint16& number );
        U8String ToUTF8( const uint32& number );
        U8String ToUTF8( const uint64& number );
        U8String ToUTF8( const float& number );
        U8String ToUTF8( const double& number );

        WString ToWide( const int8& number );
        WString ToWide( const int16& number );
        WString ToWide( const int32& number );
        WString ToWide( const int64& number );
        WString ToWide( const uint8& number );
        WString ToWide( const uint16& number );
        WString ToWide( const uint32& number );
        WString ToWide( const uint64& number );
        WString ToWide( const float& number );
        WString ToWide( const double& number );

        U8String WideToUTF8( const WChar* From );

        U8String WideToUTF8( const WString& From );

        WString UTF8ToWide( const U8Char* From );

        WString UTF8ToWide( const U8String& From );

        /// TODO this needs revision if it works with unicode
        template<class T>
        inline bool CompareIgnoreCase( T A, T B )
        {
            transform( A.begin(), A.end(), A.begin(), toupper );
            transform( B.begin(), B.end(), B.begin(), toupper );

            return (A == B);
        }

        //* Reverse the characters by newline
        template<class T>
        inline void ReverseByToken( T& String, const typename T::value_type& Char )
        {
            size_t OffsetPos = 0;
            while ( OffsetPos != T::npos )
            {
                size_t NewOffsetPos = String.find( Char, OffsetPos );
                if ( NewOffsetPos == T::npos ) break;
                std::reverse( String.begin() + OffsetPos, String.begin() + NewOffsetPos );
                OffsetPos = NewOffsetPos + 1;
            }
            std::reverse( String.begin() + OffsetPos, String.end() );
        }

        //* Replace part of string with another string
        template<class T, class C, class S>
        inline bool Replace( T& String, const C& From, const S& To )
        {
            size_t StartPos = String.find( From );

            if ( StartPos == T::npos )
            {
                return false;
            }

            String.replace( StartPos, From.length(), To );
            return true;
        }

        //* Replace part of string to the end of the string with another string
        template<class T, class C, class S>
        inline bool ReplaceFromLast( T& String, const C& From, const S& To )
        {
            size_t StartPos = String.rfind( From );

            if ( StartPos == T::npos )
            {
                return false;
            }

            String.replace( StartPos, String.length() - StartPos, To );
            return true;
        }

        //* Get the last part not containing the elements in string
        template<class T, class C>
        inline bool GetLastNotOf( const T& String, T& Residue, T& Last, const C& Elements )
        {
            size_t StartPos = String.find_last_not_of( Elements );

            if ( StartPos == T::npos )
            {
                return false;
            }

            Last = String.substr( StartPos + 1 );
            Residue = String.substr( 0, StartPos + 1 );
            return true;
        }

        //* Get the first element contained in the string
        template<class T, class C>
        inline bool GetFirstOf( const T& String, T& Residue, T& Last, const C& Elements )
        {
            size_t StartPos = String.find_fisrst_of( Elements );

            if ( StartPos == T::npos )
            {
                return false;
            }

            Last = String.substr( 0, StartPos + 1 );
            Residue = String.substr( StartPos + 1 );
            return true;
        }

        template<class T, class C = T::size_type>
        inline C CountLines( const T& String, C MaxSize = static_cast<C>(-1) )
        {
            C NewLines = 0;
            const char* Char = &String[ 0 ];
            for ( int i = 0; i < MaxSize && i < String.size_(); i++ )
            {
                if ( Char[ i ] == '\n' )
                {
                    NewLines++;
                }
            }
            return NewLines;
        }

        template<typename ... Arguments>
        WString FormattedW( const WString& format, Arguments ... Args )
        {
            const WChar* formatBuffer = format.c_str();
            size_t size = sizeof( WChar ) * format.size();
            WString result;
            result.resize( size );

            while ( true )
            {
                size_t oldSize = size;
                size = std::swprintf( result.data(), size, formatBuffer, Args ...);

                if ( size < 0 || oldSize < size )
                {
                    size += oldSize + 10;
                    result.resize( size );
                }
                else
                {
                    result.resize( size );
                    break;
                }
            }

            return result;
        }

        template<typename ... Arguments>
        U8String Formatted( const U8String& format, Arguments ... Args )
        {
            const U8Char* formatBuffer = format.c_str();
            size_t size = sizeof( U8Char ) * format.size();
            U8String result;
            result.resize( size );

            while ( true )
            {
                size_t oldSize = size;
                size = std::snprintf( result.data(), size, formatBuffer, Args ...);

                if ( size < 0 || oldSize < size )
                {
                    size += oldSize + 10;
                    result.resize( size );
                }
                else
                {
                    result.resize( size );
                    break;
                }
            }

            return result;
        }

        template<typename ... Arguments>
        WString FormattedW( const WChar* format, Arguments ... Args )
        {
            size_t size = std::wcslen( format );
            WString result;
            result.resize( size );

            while ( true )
            {
                size_t oldSize = size;
                size = std::swprintf( result.data(), size, format, Args ... );

                if ( size < 0 || oldSize < size )
                {
                    size += oldSize + 25;
                    result.resize( size );
                }
                else
                {
                    result.resize( size );
                    break;
                }
            }

            return result;
        }

        template<typename ... Arguments>
        U8String Formatted( const U8Char* format, Arguments ... Args )
        {
            size_t size = std::strlen( format );
            U8String result;
            result.resize( size );

            while ( true )
            {
                size_t oldSize = size;
                size = std::snprintf( result.data(), size, format, Args ...);

                if ( size < 0 || oldSize < size )
                {
                    size += oldSize + 25;
                    result.resize( size );
                }
                else
                {
                    result.resize( size );
                    break;
                }
            }

            return result;
        }


        template<class Num>
        inline WString FormatUnitW( const Num& number, const int& Decimals )
        {
            double precisionNumber = (double)number;
            WString suffix = L"";
            if ( precisionNumber > 1e3 && precisionNumber <= 1e6 )
            {
                suffix = L'k';
                precisionNumber /= 1e3;
            }
            else
                if ( precisionNumber > 1e6 && precisionNumber <= 1e9 )
                {
                    suffix = L'M';
                    precisionNumber /= 1e6;
                }
                else
                    if ( precisionNumber > 1e9 && precisionNumber <= 1e12 )
                    {
                        suffix = L'G';
                        precisionNumber /= 1e9;
                    }
                    else
                        if ( precisionNumber > 1e12 )
                        {
                            suffix = L'T';
                            precisionNumber /= 1e12;
                        }

            if ( int( precisionNumber ) == precisionNumber )
            {
                return FormattedW( L"%d%s", (int)precisionNumber, suffix.c_str() );
            }
            else
            {
                return FormattedW( L"%." + std::to_wstring( Decimals ) + L"f%s", precisionNumber, suffix.c_str() );
            }
        }

        template<class Num>
        inline U8String FormatUnit( const Num& number, const int& decimals )
        {
            double precisionNumber = (double)number;
            U8String suffix{};
            if ( precisionNumber > 1e3 && precisionNumber <= 1e6 )
            {
                suffix = 'k';
                precisionNumber /= 1e3;
            }
            else
                if ( precisionNumber > 1e6 && precisionNumber <= 1e9 )
                {
                    suffix = 'M';
                    precisionNumber /= 1e6;
                }
                else
                    if ( precisionNumber > 1e9 && precisionNumber <= 1e12 )
                    {
                        suffix = 'G';
                        precisionNumber /= 1e9;
                    }
                    else
                        if ( precisionNumber > 1e12 )
                        {
                            suffix = 'T';
                            precisionNumber /= 1e12;
                        }

            if ( int( precisionNumber ) == precisionNumber )
            {
                return Formatted( "%d%s", (int)precisionNumber, suffix.c_str() );
            }
            else
            {
                return Formatted( "%." + Text::ToUTF8( decimals ) + "f%s", precisionNumber, suffix.c_str() );
            }
        }

        template<class Num>
        inline WString FormatDataW( const Num& number, const int& MaxDecimals )
        {
            double precisionNumber = (double)number;
            WString suffix = L"b";
            if ( precisionNumber > 1 << 10 && precisionNumber <= 1 << 20 )
            {
                suffix = L"kb";
                precisionNumber /= 1 << 10;
            }
            else
                if ( precisionNumber > 1 << 20 && precisionNumber <= 1 << 30 )
                {
                    suffix = L"Mb";
                    precisionNumber /= 1 << 20;
                }
                else
                    if ( precisionNumber > 1 << 30 && precisionNumber <= 1ull << 40 )
                    {
                        suffix = L"Gb";
                        precisionNumber /= 1 << 30;
                    }
                    else
                        if ( precisionNumber > 1ull << 40 )
                        {
                            suffix = L"Tb";
                            precisionNumber /= 1ull << 40;
                        }

            if ( int( precisionNumber ) == precisionNumber )
            {
                return FormattedW( L"%d%s", (int)precisionNumber, suffix.c_str() );
            }
            else
            {
                return FormattedW( L"%." + Text::ToWide( MaxDecimals ) + L"f%s", precisionNumber, suffix.c_str() );
            }
        }

        template<class Num>
        inline U8String FormatData( const Num& number, const int& maxDecimals )
        {
            double precisionNumber = (double)number;
            U8String suffix = "b";
            if ( precisionNumber > 1 << 10 && precisionNumber <= 1 << 20 )
            {
                suffix = "kb";
                precisionNumber /= 1 << 10;
            }
            else
                if ( precisionNumber > 1 << 20 && precisionNumber <= 1 << 30 )
                {
                    suffix = "Mb";
                    precisionNumber /= 1 << 20;
                }
                else
                    if ( precisionNumber > 1 << 30 && precisionNumber <= 1ull << 40 )
                    {
                        suffix = "Gb";
                        precisionNumber /= 1 << 30;
                    }
                    else
                        if ( precisionNumber > 1ull << 40 )
                        {
                            suffix = "Tb";
                            precisionNumber /= 1ull << 40;
                        }

            if ( int( precisionNumber ) == precisionNumber )
            {
                return Formatted( "%d%s", (int)precisionNumber, suffix.c_str() );
            }
            else
            {
                return Formatted( "%." + Text::ToUTF8( maxDecimals ) + "f%s", precisionNumber, suffix.c_str() );
            }
        }
    }
}