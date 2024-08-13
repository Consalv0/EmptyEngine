#pragma once

#include "CoreTypes.h"

namespace EE
{
    namespace Math
    {
        template <typename T>
        struct TVector4
        {
            static_assert(IsFixedOrFloatingPoint<T>, "T must be floating point.");

        public:
            union
            {
                struct { T x, y, z, w; };
                struct { T r, g, b, a; };
            };

            HOST_DEVICE FORCEINLINE constexpr TVector4();
            HOST_DEVICE FORCEINLINE constexpr TVector4( const TVector2<T>& vector );
            HOST_DEVICE FORCEINLINE constexpr TVector4( const TVector3<T>& vector );
            HOST_DEVICE FORCEINLINE constexpr TVector4( const TVector3<T>& vector, const T& w );
            HOST_DEVICE FORCEINLINE constexpr TVector4( const TVector4<T>& vector );
            HOST_DEVICE FORCEINLINE constexpr TVector4( const T& value );
            HOST_DEVICE FORCEINLINE constexpr TVector4( const T& x, const T& y, const T& z );
            HOST_DEVICE FORCEINLINE constexpr TVector4( const T& x, const T& y, const T& z, const T& w );

            HOST_DEVICE inline T Magnitude() const;
            HOST_DEVICE inline T MagnitudeSquared() const;
            HOST_DEVICE inline void Normalize();
            HOST_DEVICE inline TVector4 Normalized() const;

            HOST_DEVICE FORCEINLINE T Dot( const TVector4& other ) const;
            HOST_DEVICE FORCEINLINE static T Dot( const TVector4& a, const TVector4& b );
            HOST_DEVICE FORCEINLINE static TVector4 Lerp( const TVector4& start, const TVector4& end, T t );

            HOST_DEVICE inline T& operator[]( unsigned char i );
            HOST_DEVICE inline T const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue() const;

            HOST_DEVICE FORCEINLINE bool operator==( const TVector4& other ) const;
            HOST_DEVICE FORCEINLINE bool operator!=( const TVector4& other ) const;

            HOST_DEVICE FORCEINLINE TVector4 operator+( const TVector4& other ) const;
            HOST_DEVICE FORCEINLINE TVector4 operator-( const TVector4& other ) const;
            HOST_DEVICE FORCEINLINE TVector4 operator-( void ) const;
            HOST_DEVICE FORCEINLINE TVector4 operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TVector4 operator/( const T& value ) const;
            HOST_DEVICE FORCEINLINE TVector4 operator*( const TVector4& other ) const;
            HOST_DEVICE FORCEINLINE TVector4 operator/( const TVector4& other ) const;

            HOST_DEVICE FORCEINLINE TVector4& operator+=( const TVector4& other );
            HOST_DEVICE FORCEINLINE TVector4& operator-=( const TVector4& other );
            HOST_DEVICE FORCEINLINE TVector4& operator*=( const TVector4& other );
            HOST_DEVICE FORCEINLINE TVector4& operator/=( const TVector4& other );
            HOST_DEVICE FORCEINLINE TVector4& operator*=( const T& value );
            HOST_DEVICE FORCEINLINE TVector4& operator/=( const T& value );

            template <typename R>
            HOST_DEVICE FORCEINLINE friend TVector4 operator*( R value, const TVector4& vector )
            { return vector.operator*( value ); }
            template <typename R>
            HOST_DEVICE FORCEINLINE friend TVector4 operator/( R value, const TVector4& vector )
            { return TVector2( value / vector.x, value / vector.y, value / vector.z, value / vector.w ); }

            template<typename R>
            explicit TVector4<T>( const TVector4<R>& other ) : TVector4<T>( (T)other.x, (T)other.y, (T)other.z, (T)other.w ) {}
        };
    }
}

#include "../Math/Vector4.inl"
