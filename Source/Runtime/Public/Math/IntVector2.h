#pragma once

#include "CoreTypes.h"

namespace EE
{
    namespace Math
    {
        template <typename T>
        struct TIntVector2
        {
        public:
            union
            {
                struct { T x, y; };
                struct { T u, v; };
            };

            HOST_DEVICE FORCEINLINE constexpr TIntVector2();
            HOST_DEVICE FORCEINLINE constexpr TIntVector2( const TIntVector2& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector2( const TIntVector2<R>& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector2( const TIntVector3<R>& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector2( const TVector2<R>& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector2( const TVector3<R>& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector2( const TVector4<R>& vector );
            HOST_DEVICE FORCEINLINE constexpr TIntVector2( const T& value );
            HOST_DEVICE FORCEINLINE constexpr TIntVector2( const T& x, const T& y );

            template <typename R>
            HOST_DEVICE FORCEINLINE R Magnitude() const;
            HOST_DEVICE FORCEINLINE T MagnitudeSquared() const;

            HOST_DEVICE FORCEINLINE T Dot( const TIntVector2& other ) const;
            HOST_DEVICE FORCEINLINE static T Dot( const TIntVector2& a, const TIntVector2& b );
            HOST_DEVICE FORCEINLINE T Cross( const TIntVector2& other ) const;
            HOST_DEVICE FORCEINLINE static T Cross( const TIntVector2& a, const TIntVector2& b );

            template <typename R>
            HOST_DEVICE FORCEINLINE TVector2<R> FloatVector2() const;
            HOST_DEVICE FORCEINLINE T& operator[]( unsigned char i );
            HOST_DEVICE FORCEINLINE T const& operator[]( unsigned char i ) const;
            HOST_DEVICE FORCEINLINE const T* PointerToValue() const;

            HOST_DEVICE FORCEINLINE bool operator==( const TIntVector2& other );
            HOST_DEVICE FORCEINLINE bool operator!=( const TIntVector2& other );

            HOST_DEVICE FORCEINLINE TIntVector2 operator+( const TIntVector2& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector2 operator-( const TIntVector2& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector2 operator-( void ) const;
            HOST_DEVICE FORCEINLINE TIntVector2 operator*( const TIntVector2& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector2 operator/( const TIntVector2& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector2 operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TIntVector2 operator/( const T& value ) const;

            HOST_DEVICE FORCEINLINE TIntVector2& operator+=( const TIntVector2& other );
            HOST_DEVICE FORCEINLINE TIntVector2& operator-=( const TIntVector2& other );
            HOST_DEVICE FORCEINLINE TIntVector2& operator*=( const TIntVector2& other );
            HOST_DEVICE FORCEINLINE TIntVector2& operator/=( const TIntVector2& other );
            HOST_DEVICE FORCEINLINE TIntVector2& operator*=( const T& value );
            HOST_DEVICE FORCEINLINE TIntVector2& operator/=( const T& value );

            HOST_DEVICE inline friend TIntVector2 operator*( T value, const TIntVector2& vector );
            HOST_DEVICE inline friend TIntVector2 operator/( T value, const TIntVector2& vector );
        };

        template <typename T>
        using TPoint2 = TVector2<T>;
    }
}

#include "Math/IntVector2.inl"