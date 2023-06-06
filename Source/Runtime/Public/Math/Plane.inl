#pragma once

#include <math.h>

#include "Math/Plane.h"

namespace EE
{
    FORCEINLINE Plane::Plane() : x(), y(), z(), d() {}

    FORCEINLINE Plane::Plane( const float& x, const float& y, const float& z, const float& d ) : x( x ), y( y ), z( z ), d( d ) {}

    FORCEINLINE Plane::Plane( const Vector3& normalizedNormal, float distance )
    {
        x = normalizedNormal.x;
        y = normalizedNormal.y;
        z = normalizedNormal.z;
        d = distance;
    }

    FORCEINLINE Plane Plane::FromPointNormal( const Point3& point, const Vector3& normal )
    {
        Vector3 normalizedNormal = normal.Normalized();
        return Plane(
            normalizedNormal.x,
            normalizedNormal.y,
            normalizedNormal.z,
            -Vector3::Dot( point, normalizedNormal )
        );
    }

    FORCEINLINE Plane Plane::From3Points( const Point3& v0, const Point3& v1, const Point3& v2 )
    {
        Vector3 normal = Vector3::Cross( v1 - v0, v2 - v0 );
        normal.Normalize();
        FromPointNormal( v0, normal );
    }

    FORCEINLINE void Plane::Normalize()
    {
        float distance = sqrtf( x * x + y * y + z * z );
        x /= distance;
        y /= distance;
        z /= distance;
        d /= distance;
    }

    FORCEINLINE float Plane::SignedDistance( const Vector3& p ) const
    {
        return (x * p.x + y * p.y + z * p.z + d);
    }

    inline Point3 Plane::SegmentIntersection( const Point3& point1, const Point3& point2 ) const
    {
        return point1 + (point2 - point1) * ((d - Dot( point1 )) / (Dot( point2 - point1 )));
    }

    FORCEINLINE Point3 Plane::GetOrigin() const
    {
        return normal * d;
    }

    FORCEINLINE float Plane::Dot( const Point3& p ) const
    {
        return x * p.x + y * p.y + z * p.z - d;
    }

    FORCEINLINE float Plane::Dot( const Vector4& v ) const
    {
        return x * v.x + y * v.y + z * v.z + d * v.w;
    }

    FORCEINLINE float Plane::Dot( const Vector3& v ) const
    {
        return x * v.x + y * v.y + z * v.z + d;
    }

    FORCEINLINE float Plane::DotCoord( const Vector3& v ) const
    {
        return x * v.x + y * v.y + z * v.z + d;
    }

    FORCEINLINE float Plane::DotNormal( const Vector3& v ) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    FORCEINLINE Frustrum::Frustrum( ) : left(), right(), top(), bottom(), near(), far()
    {
    }

    FORCEINLINE bool Frustrum::Inside( const Point3& point ) const
    {
        return
              left.Dot( point ) > 0 &&
             right.Dot( point ) > 0 &&
            bottom.Dot( point ) > 0 &&
               top.Dot( point ) > 0 &&
               far.Dot( point ) > 0 &&
              near.Dot( point ) > 0;
    }

    FORCEINLINE Frustrum& Frustrum::FromMVP( const Matrix4x4& mvp )
    {
        const float* m = mvp.PointerToValue();
        Frustrum fustrum;

        fustrum.left   = { m[ 3 ] + m[ 0 ], m[ 7 ] + m[ 4 ], m[ 11 ] + m[  8 ], m[ 15 ] + m[ 12 ] };
        fustrum.left.Normalize();
        fustrum.right  = { m[ 3 ] - m[ 0 ], m[ 7 ] - m[ 4 ], m[ 11 ] - m[  8 ], m[ 15 ] - m[ 12 ] };
        fustrum.right.Normalize();
        fustrum.top    = { m[ 3 ] - m[ 1 ], m[ 7 ] - m[ 5 ], m[ 11 ] - m[  9 ], m[ 15 ] - m[ 13 ] };
        fustrum.top.Normalize();
        fustrum.bottom = { m[ 3 ] + m[ 1 ], m[ 7 ] + m[ 5 ], m[ 11 ] + m[  9 ], m[ 15 ] + m[ 13 ] };
        fustrum.bottom.Normalize();
        fustrum.near   = { m[ 3 ] + m[ 2 ], m[ 7 ] + m[ 6 ], m[ 11 ] + m[ 10 ], m[ 15 ] + m[ 14 ] };
        fustrum.near.Normalize();
        fustrum.far    = { m[ 3 ] - m[ 2 ], m[ 7 ] - m[ 6 ], m[ 11 ] - m[ 10 ], m[ 15 ] - m[ 14 ] };
        fustrum.far.Normalize();

        return fustrum;
    }
}