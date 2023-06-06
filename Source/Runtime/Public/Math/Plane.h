#pragma once

#include "CoreTypes.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"

namespace EE
{
    enum class EFustrumPlanes : int
    {
        FustrumPlane_Left,
        FustrumPlane_Right,
        FustrumPlane_Top,
        FustrumPlane_Bottom,
        FustrumPlane_Near,
        FustrumPlane_Far,
    };

    struct Plane
    {
    public:
        union
        {
            struct { float x, y, z, d; };
            struct { Vector3 normal; float distance; };
        };

        HOST_DEVICE FORCEINLINE Plane();
        HOST_DEVICE FORCEINLINE Plane( const float& x, const float& y, const float& z, const float& d );
        HOST_DEVICE FORCEINLINE Plane( const Vector3& normalizedNormal, float distance );

        HOST_DEVICE FORCEINLINE static Plane FromPointNormal( const Point3& point, const Vector3& normal );
        HOST_DEVICE FORCEINLINE static Plane From3Points( const Point3& v0, const Point3& v1, const Point3& v2 );

        HOST_DEVICE FORCEINLINE void Normalize();

        HOST_DEVICE FORCEINLINE float SignedDistance( const Vector3& point ) const;

        FORCEINLINE Point3 SegmentIntersection( const Point3& point1, const Point3& point2 ) const;
        
        FORCEINLINE Point3 GetOrigin() const;

        // Rreturn >0: point is in front of the plane, <0: behind, =0: on the plane */
        HOST_DEVICE FORCEINLINE float Dot( const Point3& p ) const;
        HOST_DEVICE FORCEINLINE float Dot( const Vector4& v ) const;
        HOST_DEVICE FORCEINLINE float Dot( const Vector3& v ) const;
        HOST_DEVICE FORCEINLINE float DotCoord( const Vector3& v ) const;
        HOST_DEVICE FORCEINLINE float DotNormal( const Vector3& v ) const;

    };

    struct Frustrum
    {
        union
        {
            struct { Plane left, right, top, bottom, near, far; };
            struct { Plane planes[ 6 ]; };
        };

        HOST_DEVICE FORCEINLINE Frustrum();

        FORCEINLINE bool Inside( const Point3& point ) const;

        // Construct frustum from modelview-projection transformation.
        FORCEINLINE static Frustrum& FromMVP( const Matrix4x4& mvp );
    };
}

#include "Math/Plane.inl"