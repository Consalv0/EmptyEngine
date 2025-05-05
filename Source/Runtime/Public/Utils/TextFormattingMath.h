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

        template <typename T>
        inline WString FormatMath( const Math::TQuaternion<T>& value )
        {
            return Formatted( L"{%.3f, %.3f, %.3f, %.3f}", value.w, value.x, value.y, value.z );
        }

        template <typename T>
        inline WString FormatMath( const Math::TMatrix4x4<T>& value, bool columnMajor = false )
        {
            if ( columnMajor )
            {
                return Formatted( L"{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m00, value.m10, value.m20, value.m30,
                    value.m01, value.m11, value.m21, value.m31,
                    value.m02, value.m12, value.m22, value.m32,
                    value.m03, value.m13, value.m23, value.m33
                );
            }
            else
            {
                return Formatted( L"{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m00, value.m01, value.m02, value.m03,
                    value.m10, value.m11, value.m12, value.m13,
                    value.m20, value.m21, value.m22, value.m23,
                    value.m30, value.m31, value.m32, value.m33
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

        inline NString FormatMathN( const Matrix4x4& value, bool columnMajor = false )
        {
            if ( columnMajor )
            {
                return FormattedN( "{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m00, value.m10, value.m20, value.m30,
                    value.m01, value.m11, value.m21, value.m31,
                    value.m02, value.m12, value.m22, value.m32,
                    value.m03, value.m13, value.m23, value.m33
                );
            }
            else
            {
                return FormattedN( "{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m00, value.m01, value.m02, value.m03,
                    value.m10, value.m11, value.m12, value.m13,
                    value.m20, value.m21, value.m22, value.m23,
                    value.m30, value.m31, value.m32, value.m33
                );
            }
        }
    }
}