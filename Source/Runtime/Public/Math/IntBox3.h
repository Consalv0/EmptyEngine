#pragma once

#include "Math/MathUtils.h"
#include "Math/Matrix4x4.h"
#include "Math/IntVector3.h"

namespace EE::Math
{
    template<typename T>
	struct TIntBox3
	{
        static_assert(std::is_integral_v<T>, "T must be integrer type.");

	public:
		union
		{
			struct { T left, bottom, back, right, top, front; };
			struct { T minX, minY, minZ, maxX, maxY, maxZ; };
		};

		constexpr TIntBox3()
		{
			minX = minY = minZ = MathConstants<T>::MaxValue;
			maxX = maxY = maxZ = MathConstants<T>::MinValue;
		}

		constexpr TIntBox3( T minX, T minY, T minZ, T maxX, T maxY, T maxZ )
			: minX( minX ), minY( minY ), minZ( minZ ), maxX( maxX ), maxY( maxY ), maxZ( maxZ )
		{
		}
		//* Add point to the BondingBox
		inline void Add( TIntPoint3<T> point )
		{
			minX = Math::Min( minX, point.x ); minY = Math::Min( minY, point.y ); minZ = Math::Min( minZ, point.z );
			maxX = Math::Max( maxX, point.x ); maxY = Math::Max( maxY, point.y ); maxZ = Math::Max( maxZ, point.z );
		};

		//* Get the dimensions of the bounding box
		inline TIntVector3<T> GetSize() const { return TIntVector3<T>( GetWidth(), GetHeight(), GetDepth() ); }

		//* Get the center position of the bounding box
        template<typename R>
        inline TVector3<R> GetCenter() const
        {
            static_assert(std::is_floating_point_v<R>, "R must be floating point");
            return TVector3<R>( minX + maxX, minY + maxY, minZ + maxZ ) * R(0.5);
        }

		//* Get the lower point of the bounding box
		inline TIntPoint3<T> GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ), Math::Min( front, back ) }; }

		//* Get the upper point of the bounding box
		inline TIntPoint3<T> GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ), Math::Max( front, back ) }; }

		//* Get the width of the bounding box
		inline T GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

		//* Get the height of the bounding box
		inline T GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

		//* Get the depth of the bounding box
		inline T GetDepth() const { return Math::Max( front, back ) - Math::Min( front, back ); }

		//* Get the area of the bounding box
		inline T GetArea() const { return GetWidth() * GetHeight() * GetDepth(); }

		//* Get the perimeter of the bounding box
		inline T GetPerimeter() const { return GetWidth() * T(2) + GetHeight() * T(2) + GetDepth() * T(2); }
	};
}