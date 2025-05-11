#pragma once

#include "Math/CoreMath.h"
#include "Utils/TextFormatting.h"

namespace EE
{
    namespace Text
    {
        float ParseFloat( std::string_view* text );
        
        int32 ParseInt( std::string_view* text );

        inline WString FormatMathW( const UIntVector2& value )
        {
            return FormattedW( L"{%u, %u}", value.x, value.y );
        }

        inline WString FormatMathW( const UIntVector3& value )
        {
            return FormattedW( L"{%u, %u, %u}", value.x, value.y, value.z );
        }

        inline WString FormatMathW( const Vector2f& value )
        {
            return FormattedW( L"{%.3f, %.3f}", value.x, value.y );
        }

        inline WString FormatMathW( const Vector3f& value )
        {
            return FormattedW( L"{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
        }

        inline WString FormatMathW( const Vector4f& value )
        {
            return FormattedW( L"{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
        }

        inline WString FormatMathW( const IntVector2& value )
        {
            return FormattedW( L"{%d, %d}", value.x, value.y );
        }

        inline WString FormatMathW( const IntVector3& value )
        {
            return FormattedW( L"{%d, %d, %d}", value.x, value.y, value.z );
        }

        inline WString FormatMathW( const IntVector4& value )
        {
            return FormattedW( L"{%d, %d, %d, %d}", value.x, value.y, value.z, value.w );
        }

        inline WString FormatMathW( const Vector2& value )
        {
            return FormattedW( L"{%.3f, %.3f}", value.x, value.y );
        }

        inline WString FormatMathW( const Vector3& value )
        {
            return FormattedW( L"{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
        }

        inline WString FormatMathW( const Vector4& value )
        {
            return FormattedW( L"{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
        }

        template <typename T>
        inline WString FormatMathW( const Math::TQuaternion<T>& value )
        {
            return FormattedW( L"{%.3f, %.3f, %.3f, %.3f}", value.w, value.x, value.y, value.z );
        }

        template <typename T>
        inline WString FormatMathW( const Math::TMatrix4x4<T>& value, bool columnMajor = false )
        {
            if ( columnMajor )
            {
                return FormattedW( L"{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m00, value.m10, value.m20, value.m30,
                    value.m01, value.m11, value.m21, value.m31,
                    value.m02, value.m12, value.m22, value.m32,
                    value.m03, value.m13, value.m23, value.m33
                );
            }
            else
            {
                return FormattedW( L"{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m00, value.m01, value.m02, value.m03,
                    value.m10, value.m11, value.m12, value.m13,
                    value.m20, value.m21, value.m22, value.m23,
                    value.m30, value.m31, value.m32, value.m33
                );
            }
        }

        inline U8String FormatMath( const UIntVector2& value )
        {
            return Formatted( "{%u, %u}", value.x, value.y );
        }

        inline U8String FormatMath( const UIntVector3& value )
        {
            return Formatted( "{%u, %u, %u}", value.x, value.y, value.z );
        }

        inline U8String FormatMath( const Vector2f& value )
        {
            return Formatted( "{%.3f, %.3f}", value.x, value.y );
        }

        inline U8String FormatMath( const Vector3f& value )
        {
            return Formatted( "{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
        }

        inline U8String FormatMath( const Vector4f& value )
        {
            return Formatted( "{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
        }

        inline U8String FormatMath( const IntVector2& value )
        {
            return Formatted( "{%d, %d}", value.x, value.y );
        }

        inline U8String FormatMath( const IntVector3& value )
        {
            return Formatted( "{%d, %d, %d}", value.x, value.y, value.z );
        }

        inline U8String FormatMath( const IntVector4& value )
        {
            return Formatted( "{%d, %d, %d, %d}", value.x, value.y, value.z, value.w );
        }

        template <typename T>
        inline U8String FormatMath( const Math::TVector2<T>& value )
        {
            return Formatted( "{%.3f, %.3f}", value.x, value.y );
        }

        template <typename T>
        inline U8String FormatMath( const Math::TVector3<T>& value )
        {
            return Formatted( "{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
        }

        template <typename T>
        inline U8String FormatMath( const Math::TVector4<T>& value )
        {
            return Formatted( "{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
        }

        template <typename T>
        inline U8String FormatMath( const Math::TQuaternion<T>& value )
        {
            return Formatted( "{%.3f, %.3f, %.3f, %.3f}", value.w, value.x, value.y, value.z );
        }

        template <typename T>
        inline U8String FormatMath( const Math::TMatrix4x4<T>& value, bool columnMajor = false )
        {
            if ( columnMajor )
            {
                return Formatted( "{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m00, value.m10, value.m20, value.m30,
                    value.m01, value.m11, value.m21, value.m31,
                    value.m02, value.m12, value.m22, value.m32,
                    value.m03, value.m13, value.m23, value.m33
                );
            }
            else
            {
                return Formatted( "{{%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}, {%.3f, %.3f, %.3f, %.3f}}",
                    value.m00, value.m01, value.m02, value.m03,
                    value.m10, value.m11, value.m12, value.m13,
                    value.m20, value.m21, value.m22, value.m23,
                    value.m30, value.m31, value.m32, value.m33
                );
            }
        }
    }
}