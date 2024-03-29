#pragma once

#include "Math/MathUtils.h"
#include "Math/IntVector2.h"

namespace EE::Math
{
    template<typename T>
	struct TIntBox2
	{
        static_assert(std::is_integral_v<T>, "T must be integrer type.");

	public:
		union
		{
			struct { T left, bottom, right, top; };
			struct { T minX, minY, maxX, maxY; };
		};

		TIntBox2() = default;

		TIntBox2( const T& minX, const T& minY, const T& width, const T& height )
			: minX( minX ), minY( minY ), maxX( minX + width ), maxY( minY + height )
		{
		}

		//* Get the lower point of the bounding box
		inline TIntPoint2<T> GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ) }; }

		//* Get the upper point of the bounding box
		inline TIntPoint2<T> GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ) }; }

        //* Get the center position of the bounding box
        template<typename R>
        inline TVector2<R> GetCenter() const
        {
            static_assert(std::is_floating_point_v<R>, "R must be floating point");
            return TVector2<R>( minX + maxX, minY + maxY ) * R( 0.5 );
        }

		//* Get the width of the bounding box
		inline T GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

		//* Get the height of the bounding box
		inline T GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

		//* Get the area of the bounding box
		inline T GetArea() const { return GetWidth() * GetHeight(); }

		//* Get the perimeter of the bounding box
		inline T GetPerimeter() const { return GetWidth() * T(2) + GetHeight() * T(2); }
	};
}