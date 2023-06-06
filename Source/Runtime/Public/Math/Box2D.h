#pragma once

#include "Math/MathUtils.h"
#include "Math/Vector2.h"

namespace EE
{
	struct Box2D
	{
	public:
		union
		{
			struct { float left, bottom, right, top; };
			struct { float minX, minY, maxX, maxY; };
		};

		Box2D() = default;

		Box2D( const float& minX, const float& minY, const float& maxX, const float& maxY )
			: minX( minX ), minY( minY ), maxX( maxX ), maxY( maxY )
		{
		}

		//* Get the lower point of the bounding box
		inline Point2 GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ) }; }

		//* Get the upper point of the bounding box
		inline Point2 GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ) }; }

		//* Get the width of the bounding box
		inline float GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

		//* Get the height of the bounding box
		inline float GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

		//* Get the area of the bounding box
		inline float GetArea() const { return GetWidth() * GetHeight(); }

		//* Get the perimeter of the bounding box
		inline float GetPerimeter() const { return GetWidth() * 2.F + GetHeight() * 2.F; }

        //* Increase bounds by adding a point
        inline void AddPoint( const Vector2& point ) 
        {
            minX = Math::Min( minX, point.x ); minY = Math::Min( minY, point.y );
            maxX = Math::Max( maxX, point.x ); maxY = Math::Max( maxY, point.y );
        }
	};

	typedef Box2D BoundingBox2D;
}