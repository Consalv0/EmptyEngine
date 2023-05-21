#pragma once

#include "Math/MathUtils.h"
#include "Math/Matrix4x4.h"
#include "Math/IntVector3.h"

namespace EEngine
{
	struct IntBox3D
	{
	public:
		union
		{
			struct { int left, bottom, back, right, top, front; };
			struct { int minX, minY, minZ, maxX, maxY, maxZ; };
		};

		IntBox3D()
		{
			minX = minY = minZ = Math::MaxNumber;
			maxX = maxY = maxZ = -Math::MaxNumber;
		}

		IntBox3D( int minX, int minY, int minZ, int maxX, int maxY, int maxZ )
			: minX( minX ), minY( minY ), minZ( minZ ), maxX( maxX ), maxY( maxY ), maxZ( maxZ )
		{
		}
		//* Add point to the BondingBox
		inline void Add( IntPoint3 point )
		{
			minX = Math::Min( minX, point.x ); minY = Math::Min( minY, point.y ); minZ = Math::Min( minZ, point.z );
			maxX = Math::Max( maxX, point.x ); maxY = Math::Max( maxY, point.y ); maxZ = Math::Max( maxZ, point.z );
		};

		//* Get the dimensions of the bounding box
		inline IntVector3 GetSize() const { return IntVector3( GetWidth(), GetHeight(), GetDepth() ); }

		//* Get the center position of the bounding box
		inline IntVector3 GetCenter() const { return IntVector3( minX + maxX, minY + maxY, minZ + maxZ ) / 2; }

		//* Get the lower point of the bounding box
		inline IntPoint3 GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ), Math::Min( front, back ) }; }

		//* Get the upper point of the bounding box
		inline IntPoint3 GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ), Math::Max( front, back ) }; }

		//* Get the width of the bounding box
		inline int GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

		//* Get the height of the bounding box
		inline int GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

		//* Get the depth of the bounding box
		inline int GetDepth() const { return Math::Max( front, back ) - Math::Min( front, back ); }

		//* Get the area of the bounding box
		inline int GetArea() const { return GetWidth() * GetHeight() * GetDepth(); }

		//* Get the perimeter of the bounding box
		inline int GetPerimeter() const { return GetWidth() * 2 + GetHeight() * 2 + GetDepth() * 2; }

	};
}