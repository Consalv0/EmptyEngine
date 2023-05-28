#pragma once

#include "CoreTypes.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace EE
{
	struct Plane
	{
	public:
		union
		{
			struct { float x, y, z, d; };
			struct { Vector3 Normal; float Distance; };
		};

		HOST_DEVICE FORCEINLINE Plane();
		HOST_DEVICE FORCEINLINE Plane( const float& x, const float& y, const float& z, const float& d );
		HOST_DEVICE FORCEINLINE Plane( const Vector3& NormalizedNormal, float InD );

		HOST_DEVICE FORCEINLINE static Plane FromPointNormal( const Point3& point, const Vector3& Normal );
		HOST_DEVICE FORCEINLINE static Plane From3Points( const Point3& v0, const Point3& v1, const Point3& v2 );

		HOST_DEVICE FORCEINLINE void Normalize();

		HOST_DEVICE FORCEINLINE float SignedDistance( const Vector3& p ) const;

		HOST_DEVICE FORCEINLINE static float Dot( const Plane& p, const Vector4& v );
		HOST_DEVICE FORCEINLINE static float Dot( const Plane& p, const Vector3& v );
		HOST_DEVICE FORCEINLINE static float DotCoord( const Plane& p, const Vector3& v );
		HOST_DEVICE FORCEINLINE static float DotNormal( const Plane& p, const Vector3& v );

	};
}

#include "Math/Plane.inl"