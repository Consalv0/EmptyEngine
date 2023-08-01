#pragma once

#include "Math/CoreMath.h"
#include "Utils/TextFormatting.h"

namespace EE
{
	namespace Text
	{
		inline WString FormatMath( const TIntVector2& value )
		{
			return Formatted( L"{%d, %d}", value.x, value.y );
		}

		inline WString FormatMath( const TIntVector3& value )
		{
			return Formatted( L"{%d, %d, %d}", value.x, value.y, value.z );
		}

		inline WString FormatMath( const TQuaternion& value )
		{
			return Formatted( L"{%.3f, %.3f, %.3f, %.3f}", value.w, value.x, value.y, value.z );
		}

		inline WString FormatMath( const TVector2& value )
		{
			return Formatted( L"{%.3f, %.3f}", value.x, value.y );
		}

		inline WString FormatMath( const TVector3& value )
		{
			return Formatted( L"{%.3f, %.3f, %.3f}", value.x, value.y, value.z );
		}

		inline WString FormatMath( const TVector4& value )
		{
			return Formatted( L"{%.3f, %.3f, %.3f, %.3f}", value.x, value.y, value.z, value.w );
		}

		inline WString FormatMath( const TMatrix4x4& value, bool ColumnMajor = false )
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