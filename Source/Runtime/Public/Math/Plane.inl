#pragma once

#include <math.h>

#include "Math/Plane.h"

namespace EE
{
    FORCEINLINE Plane::Plane() : x(), y(), z(), d() {}

    FORCEINLINE Plane::Plane( const Plane& other ) :
        x( other.x ), y( other.y ), z( other.z ), d( other.d )
    { }

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

    FORCEINLINE float Plane::DotPoint( const Point3& p ) const
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

    FORCEINLINE Frustrum::Frustrum( const Frustrum& other )
        : left( other.left ), right( other.right ), top( other.top ), bottom( other.bottom ), near( other.near ), far( other.far )
    {
    }

    FORCEINLINE Frustrum::Frustrum( const Plane& left, const Plane& right, const Plane& top, const Plane& bottom, const Plane& near , const Plane& far )
        : left( left ), right( right ), top( top ), bottom( bottom ), near( near ), far( far)
    {
    }

    FORCEINLINE bool Frustrum::Inside( const Point3& point ) const
    {
        return
              left.DotPoint( point ) < 0 &&
             right.DotPoint( point ) < 0 &&
            bottom.DotPoint( point ) < 0 &&
               top.DotPoint( point ) < 0 &&
               far.DotPoint( point ) < 0 &&
              near.DotPoint( point ) < 0;
    }

    FORCEINLINE Frustrum Frustrum::FromMVP( const Matrix4x4& mvp )
    {
        Frustrum fustrum;

        fustrum.left   = { mvp.m03 + mvp.m00, mvp.m13 + mvp.m10, mvp.m23 + mvp.m20, mvp.m33 + mvp.m30 };
        fustrum.right  = { mvp.m03 - mvp.m00, mvp.m13 - mvp.m10, mvp.m23 - mvp.m20, mvp.m33 - mvp.m30 };
        fustrum.top    = { mvp.m03 - mvp.m01, mvp.m13 - mvp.m11, mvp.m23 - mvp.m21, mvp.m33 - mvp.m31 };
        fustrum.bottom = { mvp.m03 + mvp.m01, mvp.m13 + mvp.m11, mvp.m23 + mvp.m21, mvp.m33 + mvp.m31 };
        fustrum.near   = { mvp.m03 + mvp.m02, mvp.m13 + mvp.m12, mvp.m23 + mvp.m22, mvp.m33 + mvp.m32 };
        fustrum.far    = { mvp.m03 - mvp.m02, mvp.m13 - mvp.m12, mvp.m23 - mvp.m22, mvp.m33 - mvp.m32 };
        fustrum.left.Normalize();
        fustrum.right.Normalize();
        fustrum.top.Normalize();
        fustrum.bottom.Normalize();
        fustrum.near.Normalize();
        fustrum.far.Normalize();

        return fustrum;
    }
}