#pragma once

#include "CoreTypes.h"
#include <algorithm>

namespace EE
{
    namespace Text
    {
        NString ToNarrow( const int8& number );
        NString ToNarrow( const int16& number );
        NString ToNarrow( const int32& number );
        NString ToNarrow( const int64& number );
        NString ToNarrow( const uint8& number );
        NString ToNarrow( const uint16& number );
        NString ToNarrow( const uint32& number );
        NString ToNarrow( const uint64& number );
        NString ToNarrow( const float& number );
        NString ToNarrow( const double& number );

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

        NString WideToNarrow( const WChar* From );

        WString NarrowToWide( const NChar* From );

        NString WideToNarrow( const WString& From );

        WString NarrowToWide( const NString& From );

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
        WString Formatted( const WString& format, Arguments ... Args )
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
        NString FormattedN( const NString& format, Arguments ... Args )
        {
            const NChar* formatBuffer = format.c_str();
            size_t size = sizeof( NChar ) * format.size();
            NString result;
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
        WString Formatted( const WChar* format, Arguments ... Args )
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
        NString FormattedN( const NChar* format, Arguments ... Args )
        {
            size_t size = std::strlen( format );
            NString result;
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
        inline WString FormatUnit( const Num& number, const int& Decimals )
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
                return Formatted( L"%d%s", (int)precisionNumber, suffix.c_str() );
            }
            else
            {
                return Formatted( L"%." + std::to_wstring( Decimals ) + L"f%s", precisionNumber, suffix.c_str() );
            }
        }

        template<class Num>
        inline NString FormatUnitN( const Num& number, const int& Decimals )
        {
            double PrecisionNumber = (double)number;
            NString suffix{};
            if ( PrecisionNumber > 1e3 && PrecisionNumber <= 1e6 )
            {
                suffix = 'k';
                PrecisionNumber /= 1e3;
            }
            else
                if ( PrecisionNumber > 1e6 && PrecisionNumber <= 1e9 )
                {
                    suffix = 'M';
                    PrecisionNumber /= 1e6;
                }
                else
                    if ( PrecisionNumber > 1e9 && PrecisionNumber <= 1e12 )
                    {
                        suffix = 'G';
                        PrecisionNumber /= 1e9;
                    }
                    else
                        if ( PrecisionNumber > 1e12 )
                        {
                            suffix = 'T';
                            PrecisionNumber /= 1e12;
                        }

            if ( int( PrecisionNumber ) == PrecisionNumber )
            {
                return Formatted( "%d%s", (int)PrecisionNumber, suffix.c_str() );
            }
            else
            {
                return Formatted( "%." + std::to_string( Decimals ) + "f%s", PrecisionNumber, suffix.c_str() );
            }
        }

        template<class Num>
        inline WString FormatData( const Num& number, const int& MaxDecimals )
        {
            double PrecisionNumber = (double)number;
            WString Suffix = L"b";
            if ( PrecisionNumber > 1 << 10 && PrecisionNumber <= 1 << 20 )
            {
                Suffix = L"kb";
                PrecisionNumber /= 1 << 10;
            }
            else
                if ( PrecisionNumber > 1 << 20 && PrecisionNumber <= 1 << 30 )
                {
                    Suffix = L"Mb";
                    PrecisionNumber /= 1 << 20;
                }
                else
                    if ( PrecisionNumber > 1 << 30 && PrecisionNumber <= (size_t)1 << 40 )
                    {
                        Suffix = L"Gb";
                        PrecisionNumber /= 1 << 30;
                    }
                    else
                        if ( PrecisionNumber > (size_t)1 << 40 )
                        {
                            Suffix = L"Tb";
                            PrecisionNumber /= (size_t)1 << 40;
                        }

            if ( int( PrecisionNumber ) == PrecisionNumber )
            {
                return Formatted( L"%d%s", (int)PrecisionNumber, Suffix.c_str() );
            }
            else
            {
                return Formatted( L"%." + std::to_wstring( MaxDecimals ) + L"f%s", PrecisionNumber, Suffix.c_str() );
            }
        }

        template<class Num>
        inline NString FormatDataN( const Num& number, const int& maxDecimals )
        {
            double precisionNumber = (double)number;
            NString suffix = "b";
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
                    if ( precisionNumber > 1 << 30 && precisionNumber <= (size_t)1 << 40 )
                    {
                        suffix = "Gb";
                        precisionNumber /= 1 << 30;
                    }
                    else
                        if ( precisionNumber > (size_t)1 << 40 )
                        {
                            suffix = "Tb";
                            precisionNumber /= (size_t)1 << 40;
                        }

            if ( int( precisionNumber ) == precisionNumber )
            {
                return Formatted( "%d%s", (int)precisionNumber, suffix.c_str() );
            }
            else
            {
                return Formatted( "%." + std::to_string( maxDecimals ) + "f%s", precisionNumber, suffix.c_str() );
            }
        }
    }
}