#pragma once

namespace EE
{
    namespace Math
    {
        template <typename T>
        struct TIntVector3
        {
        public:
            union
            {
                struct { T x, y, z; };
                struct { T r, g, b; };
            };

            HOST_DEVICE FORCEINLINE constexpr TIntVector3();
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const TIntVector3& vector );
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const TIntVector2<T>& vector );
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const TIntVector2<T>& vector, const T& z );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const TVector2<R>& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const TVector3<R>& vector );
            template <typename R>
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const TVector4<R>& vector );
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const T& value );
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const T& x, const T& y, const T& z );
            HOST_DEVICE FORCEINLINE constexpr TIntVector3( const T& x, const T& y );

            template <typename R>
            HOST_DEVICE inline R Magnitude() const;
            HOST_DEVICE inline T MagnitudeSquared() const;

            HOST_DEVICE FORCEINLINE TIntVector3 Cross( const TIntVector3& other ) const;
            HOST_DEVICE FORCEINLINE T Dot( const TIntVector3& other ) const;

            template <typename V>
            HOST_DEVICE inline V ToFloat() const;
            HOST_DEVICE inline T& operator[]( unsigned char i );
            HOST_DEVICE inline T const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue() const;

            HOST_DEVICE FORCEINLINE bool operator==( const TIntVector3& other ) const;
            HOST_DEVICE FORCEINLINE bool operator!=( const TIntVector3& other ) const;

            HOST_DEVICE FORCEINLINE TIntVector3 operator+( const TIntVector3& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector3 operator-( const TIntVector3& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector3 operator-( void ) const;
            HOST_DEVICE FORCEINLINE TIntVector3 operator*( const TIntVector3& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector3 operator/( const TIntVector3& other ) const;
            HOST_DEVICE FORCEINLINE TIntVector3 operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TIntVector3 operator/( const T& value ) const;

            HOST_DEVICE FORCEINLINE TIntVector3& operator+=( const TIntVector3& other );
            HOST_DEVICE FORCEINLINE TIntVector3& operator-=( const TIntVector3& other );
            HOST_DEVICE FORCEINLINE TIntVector3& operator*=( const TIntVector3& other );
            HOST_DEVICE FORCEINLINE TIntVector3& operator/=( const TIntVector3& other );
            HOST_DEVICE FORCEINLINE TIntVector3& operator*=( const T& value );
            HOST_DEVICE FORCEINLINE TIntVector3& operator/=( const T& value );

            HOST_DEVICE inline friend TIntVector3 operator*( T value, const TIntVector3& vector );
            HOST_DEVICE inline friend TIntVector3 operator/( T value, const TIntVector3& vector );
        };
    }
}

#include "Math/IntVector3.inl"

