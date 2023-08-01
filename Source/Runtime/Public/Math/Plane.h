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

    namespace Math
    {
        template <typename T>
        struct TPlane
        {
        public:
            union
            {
                struct { T x, y, z, d; };
                struct { TVector3<T> normal; T distance; };
            };

            HOST_DEVICE FORCEINLINE TPlane();
            HOST_DEVICE FORCEINLINE TPlane( const TPlane& other );
            HOST_DEVICE FORCEINLINE TPlane( const T& x, const T& y, const T& z, const T& d );
            HOST_DEVICE FORCEINLINE TPlane( const TVector3<T>& normalizedNormal, T distance );

            HOST_DEVICE FORCEINLINE static TPlane FromPointNormal( const TPoint3<T>& point, const TVector3<T>& normal );
            HOST_DEVICE FORCEINLINE static TPlane From3Points( const TPoint3<T>& v0, const TPoint3<T>& v1, const TPoint3<T>& v2 );

            HOST_DEVICE FORCEINLINE void Normalize();

            HOST_DEVICE FORCEINLINE T SignedDistance( const TVector3<T>& point ) const;

            FORCEINLINE TPoint3<T> SegmentIntersection( const TPoint3<T>& point1, const TPoint3<T>& point2 ) const;

            FORCEINLINE TPoint3<T> GetOrigin() const;

            // Rreturn >0: point is in front of the plane, <0: behind, =0: on the plane */
            HOST_DEVICE FORCEINLINE T DotPoint( const TPoint3<T>& p ) const;
            HOST_DEVICE FORCEINLINE T Dot( const TVector4<T>& v ) const;
            HOST_DEVICE FORCEINLINE T Dot( const TVector3<T>& v ) const;
            HOST_DEVICE FORCEINLINE T DotCoord( const TVector3<T>& v ) const;
            HOST_DEVICE FORCEINLINE T DotNormal( const TVector3<T>& v ) const;

        };

        template <typename T>
        struct TFrustrum
        {
            union
            {
                struct { TPlane<T> left, right, top, bottom, near, far; };
                struct { TPlane<T> planes[ 6 ]; };
            };

            HOST_DEVICE FORCEINLINE TFrustrum();
            HOST_DEVICE FORCEINLINE TFrustrum( const TFrustrum& other );
            HOST_DEVICE FORCEINLINE TFrustrum( const TPlane<T>& left, const TPlane<T>& right, const TPlane<T>& top, const TPlane<T>& bottom, const TPlane<T>& near, const TPlane<T>& far );

            FORCEINLINE bool Inside( const TPoint3<T>& point ) const;

            // Construct frustum from modelview-projection transformation.
            FORCEINLINE static TFrustrum FromMVP( const TMatrix4x4<T>& mvp );
        };
    }
}

#include "Math/Plane.inl"