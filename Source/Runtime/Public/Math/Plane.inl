#pragma once

#include <math.h>

#include "Math/Plane.h"

namespace EE::Math
{
    template <typename T>
    FORCEINLINE TPlane<T>::TPlane() : x(), y(), z(), d() {}

    template <typename T>
    FORCEINLINE TPlane<T>::TPlane( const TPlane<T>& other ) :
        x( other.x ), y( other.y ), z( other.z ), d( other.d )
    { }

    template <typename T>
    FORCEINLINE TPlane<T>::TPlane( const T& x, const T& y, const T& z, const T& d ) : x( x ), y( y ), z( z ), d( d ) {}

    template <typename T>
    FORCEINLINE TPlane<T>::TPlane( const TVector3<T>& normalizedNormal, T distance )
    {
        x = normalizedNormal.x;
        y = normalizedNormal.y;
        z = normalizedNormal.z;
        d = distance;
    }

    template <typename T>
    FORCEINLINE TPlane<T> TPlane<T>::FromPointNormal( const TPoint3<T>& point, const TVector3<T>& normal )
    {
        TVector3<T> normalizedNormal = normal.Normalized();
        return TPlane(
            normalizedNormal.x,
            normalizedNormal.y,
            normalizedNormal.z,
            -TVector3<T>::Dot( point, normalizedNormal )
        );
    }

    template <typename T>
    FORCEINLINE TPlane<T> TPlane<T>::From3Points( const TPoint3<T>& v0, const TPoint3<T>& v1, const TPoint3<T>& v2 )
    {
        TVector3<T> normal = TVector3<T>::Cross( v1 - v0, v2 - v0 );
        normal.Normalize();
        FromPointNormal( v0, normal );
    }

    template <typename T>
    FORCEINLINE void TPlane<T>::Normalize()
    {
        T distance = std::sqrt( x * x + y * y + z * z );
        x /= distance;
        y /= distance;
        z /= distance;
        d /= distance;
    }

    template <typename T>
    FORCEINLINE T TPlane<T>::SignedDistance( const TVector3<T>& p ) const
    {
        return (x * p.x + y * p.y + z * p.z + d);
    }

    template <typename T>
    inline TPoint3<T> TPlane<T>::SegmentIntersection( const TPoint3<T>& point1, const TPoint3<T>& point2 ) const
    {
        return point1 + (point2 - point1) * ((d - Dot( point1 )) / (Dot( point2 - point1 )));
    }

    template <typename T>
    FORCEINLINE TPoint3<T> TPlane<T>::GetOrigin() const
    {
        return normal * d;
    }

    template <typename T>
    FORCEINLINE T TPlane<T>::DotPoint( const TPoint3<T>& p ) const
    {
        return x * p.x + y * p.y + z * p.z - d;
    }

    template <typename T>
    FORCEINLINE T TPlane<T>::Dot( const TVector4<T>& v ) const
    {
        return x * v.x + y * v.y + z * v.z + d * v.w;
    }

    template <typename T>
    FORCEINLINE T TPlane<T>::Dot( const TVector3<T>& v ) const
    {
        return x * v.x + y * v.y + z * v.z + d;
    }

    template <typename T>
    FORCEINLINE T TPlane<T>::DotCoord( const TVector3<T>& v ) const
    {
        return x * v.x + y * v.y + z * v.z + d;
    }

    template <typename T>
    FORCEINLINE T TPlane<T>::DotNormal( const TVector3<T>& v ) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    template <typename T>
    FORCEINLINE TFrustrum<T>::TFrustrum( ) : left(), right(), top(), bottom(), near(), far()
    {
    }

    template <typename T>
    FORCEINLINE TFrustrum<T>::TFrustrum( const TFrustrum<T>& other )
        : left( other.left ), right( other.right ), top( other.top ), bottom( other.bottom ), near( other.near ), far( other.far )
    {
    }

    template <typename T>
    FORCEINLINE TFrustrum<T>::TFrustrum( const TPlane<T>& left, const TPlane<T>& right, const TPlane<T>& top, const TPlane<T>& bottom, const TPlane<T>& near , const TPlane<T>& far )
        : left( left ), right( right ), top( top ), bottom( bottom ), near( near ), far( far)
    {
    }

    template <typename T>
    FORCEINLINE bool TFrustrum<T>::Inside( const TPoint3<T>& point ) const
    {
        return
              left.DotPoint( point ) < 0 &&
             right.DotPoint( point ) < 0 &&
            bottom.DotPoint( point ) < 0 &&
               top.DotPoint( point ) < 0 &&
               far.DotPoint( point ) < 0 &&
              near.DotPoint( point ) < 0;
    }

    template <typename T>
    FORCEINLINE TFrustrum<T> TFrustrum<T>::FromMVP( const TMatrix4x4<T>& mvp )
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