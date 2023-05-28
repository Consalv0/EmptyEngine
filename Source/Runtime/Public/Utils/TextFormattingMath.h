#pragma once

#include "Math/CoreMath.h"
#include "Utils/TextFormatting.h"

namespace EE
{
	namespace Text
	{
		inline WString FormatMath( const IntVector2& value )
		{
			return Formatted( L"{%d, %d}", value.x, value.y );
		}

		inline WString FormatMath( const IntVector3& value )
		{
			return Formatted( L"{%d, %d, %d}", value.x, value.y, value.z );
		}

		inline WString FormatMath( const Quaternion& value )
		{
			return Formatted( L"{%.3f, %.3f, %.3f, %.3f}", value.w, value.x, value.y, value.z );
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
	}
}