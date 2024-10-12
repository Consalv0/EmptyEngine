#pragma once

#include "Math/CoreMath.h"
#include "Utils/TextFormatting.h"

namespace EE
{
    namespace Text
    {
        float ParseFloat( std::string_view* text );
        
        int32 ParseInt( std::string_view* text );

        inline WString FormatMath( const UIntVector2& value )
        {
            return Formatted( L"{%u, %u}", value.x, value.y );
        }

        inline WString FormatMath( const UIntVector3& value )
        {
            return Formatted( L"{%u, %u, %u}", value.x, value.y, value.z );
        }

        inline WString FormatMath( const Vector2f& value )
        {
            return Formatted( L"{%.3f, %.3f}", value.x, value.y );
        }

        inline WString FormatMath( const Vector3f& value )
        {
            return Formatted( L"{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
        }

        inline WString FormatMath( const Vector4f& value )
        {
            return Formatted( L"{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
        }

        inline WString FormatMath( const IntVector2& value )
        {
            return Formatted( L"{%d, %d}", value.x, value.y );
        }

        inline WString FormatMath( const IntVector3& value )
        {
            return Formatted( L"{%d, %d, %d}", value.x, value.y, value.z );
        }

        inline WString FormatMath( const IntVector4& value )
        {
            return Formatted( L"{%d, %d, %d, %d}", value.x, value.y, value.z, value.w );
        }

        inline WString FormatMath( const Vector2& value )
        {
            return Formatted( L"{%.3f, %.3f}", value.x, value.y );
        }

        inline WString FormatMath( const Vector3& value )
        {
            return Formatted( L"{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
        }

        inline WString FormatMath( const Vector4& value )
        {
            return Formatted( L"{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
        }

        inline WString FormatMath( const Quaternion& value )
        {
            return Formatted( L"{%.3f, %.3f, %.3f, %.3f}", value.w, value.x, value.y, value.z );
        }

        inline WString FormatMath( const Matrix4x4& value, bool ColumnMajor = false )
        {
            if ( ColumnMajor )
            {
                return Formatted( L"{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m0.x, value.m1.x, value.m2.x, value.m3.x,
                    value.m0.y, value.m1.y, value.m2.y, value.m3.y,
                    value.m0.z, value.m1.z, value.m2.z, value.m3.z,
                    value.m0.w, value.m1.w, value.m2.w, value.m3.w
                );
            }
            else
            {
                return Formatted( L"{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m0.x, value.m0.y, value.m0.z, value.m0.w,
                    value.m1.x, value.m1.y, value.m1.z, value.m1.w,
                    value.m2.x, value.m2.y, value.m2.z, value.m2.w,
                    value.m3.x, value.m3.y, value.m3.z, value.m3.w
                );
            }
        }

        inline NString FormatMathN( const UIntVector2& value )
        {
            return FormattedN( "{%u, %u}", value.x, value.y );
        }

        inline NString FormatMathN( const UIntVector3& value )
        {
            return FormattedN( "{%u, %u, %u}", value.x, value.y, value.z );
        }

        inline NString FormatMathN( const Vector2f& value )
        {
            return FormattedN( "{%.3f, %.3f}", value.x, value.y );
        }

        inline NString FormatMathN( const Vector3f& value )
        {
            return FormattedN( "{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
        }

        inline NString FormatMathN( const Vector4f& value )
        {
            return FormattedN( "{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
        }

        inline NString FormatMathN( const IntVector2& value )
        {
            return FormattedN( "{%d, %d}", value.x, value.y );
        }

        inline NString FormatMathN( const IntVector3& value )
        {
            return FormattedN( "{%d, %d, %d}", value.x, value.y, value.z );
        }

        inline NString FormatMathN( const IntVector4& value )
        {
            return FormattedN( "{%d, %d, %d, %d}", value.x, value.y, value.z, value.w );
        }

        inline NString FormatMathN( const Vector2& value )
        {
            return FormattedN( "{%.3f, %.3f}", value.x, value.y );
        }

        inline NString FormatMathN( const Vector3& value )
        {
            return FormattedN( "{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
        }

        inline NString FormatMathN( const Vector4& value )
        {
            return FormattedN( "{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
        }

        inline NString FormatMathN( const Quaternion& value )
        {
            return FormattedN( "{%.3f, %.3f, %.3f, %.3f}", value.w, value.x, value.y, value.z );
        }

        inline NString FormatMathN( const Matrix4x4& value, bool ColumnMajor = false )
        {
            if ( ColumnMajor )
            {
                return FormattedN( "{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m0.x, value.m1.x, value.m2.x, value.m3.x,
                    value.m0.y, value.m1.y, value.m2.y, value.m3.y,
                    value.m0.z, value.m1.z, value.m2.z, value.m3.z,
                    value.m0.w, value.m1.w, value.m2.w, value.m3.w
                );
            }
            else
            {
                return FormattedN( "{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m0.x, value.m0.y, value.m0.z, value.m0.w,
                    value.m1.x, value.m1.y, value.m1.z, value.m1.w,
                    value.m2.x, value.m2.y, value.m2.z, value.m2.w,
                    value.m3.x, value.m3.y, value.m3.z, value.m3.w
                );
            }
        }
    }
}