#pragma once

#include "CoreTypes.h"

namespace EE
{
    namespace Math
    {
        template <typename T>
        struct TIntVector4
        {
        public:
            union
            {
                struct { T x, y, z, w; };
                struct { T r, g, b, a; };
            };

            HOST_DEVICE FORCEINLINE constexpr TIntVector4();
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const TIntVector2<T>& vector );
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const TIntVector3<T>& vector );
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const TIntVector3<T>& vector, const T& w );
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const TIntVector4<T>& vector );

            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const TVector2<R>& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const TVector3<R>& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const TVector4<R>& vector );
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const T& value );
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const T& x, const T& y );
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const T& x, const T& y, const T& z );
            HOST_DEVICE FORCEINLINE constexpr TIntVector4( const T& x, const T& y, const T& z, const T& w );

            template <typename R>
            HOST_DEVICE inline R Magnitude() const;
            HOST_DEVICE inline T MagnitudeSquared() const;

            HOST_DEVICE FORCEINLINE T Dot( const TIntVector4& other ) const;

            template <typename V>
            HOST_DEVICE inline V ToFloat() const;
            HOST_DEVICE inline T& operator[]( unsigned char i );
            HOST_DEVICE inline T const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue() const;

            HOST_DEVICE FORCEINLINE bool operator==( const TIntVector4& other );
            HOST_DEVICE FORCEINLINE bool operator!=( const TIntVector4& other );

            HOST_DEVICE FORCEINLINE TIntVector4 operator+( const TIntVector4& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector4 operator-( const TIntVector4& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector4 operator-( void ) const;
            HOST_DEVICE FORCEINLINE TIntVector4 operator*( const TIntVector4& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector4 operator/( const TIntVector4& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector4 operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TIntVector4 operator/( const T& value ) const;

            HOST_DEVICE FORCEINLINE TIntVector4& operator+=( const TIntVector4& other );
            HOST_DEVICE FORCEINLINE TIntVector4& operator-=( const TIntVector4& other );
            HOST_DEVICE FORCEINLINE TIntVector4& operator*=( const TIntVector4& other );
            HOST_DEVICE FORCEINLINE TIntVector4& operator/=( const TIntVector4& other );
            HOST_DEVICE FORCEINLINE TIntVector4& operator*=( const T& value );
            HOST_DEVICE FORCEINLINE TIntVector4& operator/=( const T& value );

            HOST_DEVICE inline friend TIntVector4 operator*( T value, const TIntVector4& vector );
            HOST_DEVICE inline friend TIntVector4 operator/( T value, const TIntVector4& vector );

            template<typename R>
            explicit TIntVector4<T>( const TIntVector4<R>& other ) : TIntVector4<T>( (T)other.x, (T)other.y, (T)other.z, (T)other.w ) {}
        };
    }
}

#include "Math/IntVector4.inl"

