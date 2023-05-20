#pragma once

#include "Math/MathUtils.h"
#include "Math/IntVector2.h"

namespace EEngine
{
	struct IntBox2D
	{
	public:
		union
		{
			struct { int left, bottom, right, top; };
			struct { int minX, minY, maxX, maxY; };
		};

		IntBox2D() = default;

		IntBox2D( const int& minX, const int& minY, const int& maxX, const int& maxY )
			: minX( minX ), minY( minY ), maxX( maxX ), maxY( maxY )
		{
		}

		//* Get the lower point of the bounding box
		inline IntPoint2 GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ) }; }

		//* Get the upper point of the bounding box
		inline IntPoint2 GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ) }; }

		//* Get the width of the bounding box
		inline int GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

		//* Get the height of the bounding box
		inline int GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

		//* Get the area of the bounding box
		inline int GetArea() const { return GetWidth() * GetHeight(); }

		//* Get the perimeter of the bounding box
		inline int GetPerimeter() const { return GetWidth() * 2 + GetHeight() * 2; }
	};
}