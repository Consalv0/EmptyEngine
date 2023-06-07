#pragma once

#include "Math/MathUtils.h"
#include "Math/IntVector2.h"

namespace EE
{
	struct IntBox2D
	{
	public:
		union
		{
			struct { int32 left, bottom, right, top; };
			struct { int32 minX, minY, maxX, maxY; };
		};

		IntBox2D() = default;

		IntBox2D( const int32& minX, const int32& minY, const int32& width, const int32& height )
			: minX( minX ), minY( minY ), maxX( minX + width ), maxY( minY + height )
		{
		}

		//* Get the lower point of the bounding box
		inline IntPoint2 GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ) }; }

		//* Get the upper point of the bounding box
		inline IntPoint2 GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ) }; }

		//* Get the width of the bounding box
		inline int32 GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

		//* Get the height of the bounding box
		inline int32 GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

		//* Get the area of the bounding box
		inline int32 GetArea() const { return GetWidth() * GetHeight(); }

		//* Get the perimeter of the bounding box
		inline int32 GetPerimeter() const { return GetWidth() * 2 + GetHeight() * 2; }
	};
}