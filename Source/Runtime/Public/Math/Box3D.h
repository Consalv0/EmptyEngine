#pragma once

#include "Math/MathUtils.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace EEngine
{
	struct Box3D
	{
	public:
		union
		{
			struct { float left, bottom, back, right, top, front; };
			struct { float minX, minY, minZ, maxX, maxY, maxZ; };
		};

		Box3D()
		{
			minX = minY = minZ = Math::MaxNumber;
			maxX = maxY = maxZ = -Math::MaxNumber;
		}

		Box3D( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
			: minX( minX ), minY( minY ), minZ( minZ ), maxX( maxX ), maxY( maxY ), maxZ( maxZ )
		{
		}

		inline Box3D Transform( const Matrix4x4& transformation ) const
		{
			Box3D value;
			// Min-max N
			value.Add( transformation.MultiplyPoint( Vector3( minX, minY, minZ ) ) );
			value.Add( transformation.MultiplyPoint( Vector3( maxX, maxY, maxZ ) ) );
			// Min-max x
			value.Add( transformation.MultiplyPoint( Vector3( maxX, minY, minZ ) ) );
			value.Add( transformation.MultiplyPoint( Vector3( minX, maxY, maxZ ) ) );
			// Min-max y
			value.Add( transformation.MultiplyPoint( Vector3( minX, maxY, minZ ) ) );
			value.Add( transformation.MultiplyPoint( Vector3( maxX, minY, maxZ ) ) );
			// Min-max z
			value.Add( transformation.MultiplyPoint( Vector3( minX, minY, maxZ ) ) );
			value.Add( transformation.MultiplyPoint( Vector3( maxX, maxY, minZ ) ) );
			return value;
		}

		//* Add point to the BondingBox
		inline void Add( Point3 point )
		{
			minX = Math::Min( minX, point.x ); minY = Math::Min( minY, point.y ); minZ = Math::Min( minZ, point.z );
			maxX = Math::Max( maxX, point.x ); maxY = Math::Max( maxY, point.y ); maxZ = Math::Max( maxZ, point.z );
		};

		//* Get the dimensions of the bounding box
		inline Vector3 GetSize() const { return Vector3( GetWidth(), GetHeight(), GetDepth() ); }

		//* Get the center position of the bounding box
		inline Vector3 GetCenter() const { return Vector3( minX + maxX, minY + maxY, minZ + maxZ ) * .5F; }

		//* Get the lower point of the bounding box
		inline Point3 GetMinPoint() const { return { Math::Min( left, right ), Math::Min( top, bottom ), Math::Min( front, back ) }; }

		//* Get the upper point of the bounding box
		inline Point3 GetMaxPoint() const { return { Math::Max( left, right ), Math::Max( top, bottom ), Math::Max( front, back ) }; }

		//* Get the width of the bounding box
		inline float GetWidth() const { return Math::Max( left, right ) - Math::Min( left, right ); }

		//* Get the height of the bounding box
		inline float GetHeight() const { return Math::Max( top, bottom ) - Math::Min( top, bottom ); }

		//* Get the depth of the bounding box
		inline float GetDepth() const { return Math::Max( front, back ) - Math::Min( front, back ); }

		//* Get the area of the bounding box
		inline float GetArea() const { return GetWidth() * GetHeight() * GetDepth(); }

		//* Get the perimeter of the bounding box
		inline float GetPerimeter() const { return GetWidth() * 2.F + GetHeight() * 2.F + GetDepth() * 2.F; }

		// Used in frustrum computations
		inline Point3 GetPointPositive( const Vector3& normal ) const
		{
			Vector3 maxPoint = GetMaxPoint();
			Point3 result = GetMinPoint();
			if ( normal.x > 0 ) result.x = maxPoint.x;
			if ( normal.y > 0 ) result.y = maxPoint.y;
			if ( normal.z > 0 ) result.z = maxPoint.z;
			return result;
		}

		// Used in frustrum computations
		inline Point3 GetPointNegative( const Vector3& normal ) const
		{
			Vector3 maxPoint = GetMaxPoint();
			Point3 result = GetMinPoint();
			if ( normal.x < 0 ) result.x = maxPoint.x;
			if ( normal.y < 0 ) result.y = maxPoint.y;
			if ( normal.z < 0 ) result.z = maxPoint.z;
			return result;
		}

	};

	typedef Box3D BoundingBox3D;
}