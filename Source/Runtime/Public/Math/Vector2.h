#pragma once

#include "CoreTypes.h"

namespace EE
{
    namespace Math
    {
        template <typename T>
        struct TVector2
        {
            static_assert(IsFixedOrFloatingPoint<T>, "T must be floating point.");

        public:
            union
            {
                struct { T x, y; };
                struct { T u, v; };
            };

            HOST_DEVICE FORCEINLINE TVector2();
            HOST_DEVICE FORCEINLINE TVector2( const TVector2<T>& vector );
            HOST_DEVICE FORCEINLINE TVector2( const TVector3<T>& vector );
            HOST_DEVICE FORCEINLINE TVector2( const TVector4<T>& vector );
            template <typename I>
            HOST_DEVICE FORCEINLINE TVector2( const TIntVector2<I>& vector );
            template <typename I>
            HOST_DEVICE FORCEINLINE TVector2( const TIntVector3<I>& vector );
            HOST_DEVICE FORCEINLINE TVector2( const T& value );
            HOST_DEVICE FORCEINLINE TVector2( const T& x, const T& y );

            HOST_DEVICE inline T Magnitude() const;
            HOST_DEVICE inline T MagnitudeSquared() const;
            HOST_DEVICE inline void Normalize();
            HOST_DEVICE inline TVector2<T> Normalized() const;

            //* a special version of the cross product for 2D vectors (returns scalar value).
            HOST_DEVICE FORCEINLINE T Cross( const TVector2<T>& other ) const;
            HOST_DEVICE FORCEINLINE static T Cross( const TVector2<T>& a, const TVector2<T>& b );
            HOST_DEVICE FORCEINLINE T Dot( const TVector2<T>& other ) const;
            HOST_DEVICE FORCEINLINE static T Dot( const TVector2<T>& a, const TVector2<T>& b );
            HOST_DEVICE FORCEINLINE TVector2<T> Orthogonal( bool polarity ) const;
            HOST_DEVICE FORCEINLINE TVector2<T> Orthonormal( bool polarity ) const;
            HOST_DEVICE FORCEINLINE static TVector2<T> Lerp( const TVector2<T>& start, const TVector2<T>& end, T t );

            HOST_DEVICE FORCEINLINE TVector2<T> Rotate( TVector2<T>& a, const TVector2<T>& center, T radians ) const;

            HOST_DEVICE inline T& operator[]( unsigned char i );
            HOST_DEVICE inline T const& operator[]( unsigned char i ) const;
            HOST_DEVICE inline const T* PointerToValue() const;

            HOST_DEVICE FORCEINLINE bool operator==( const TVector2<T>& other ) const;
            HOST_DEVICE FORCEINLINE bool operator!() const;
            HOST_DEVICE FORCEINLINE bool operator!=( const TVector2<T>& other ) const;

            HOST_DEVICE FORCEINLINE TVector2 operator+( const TVector2<T>& other ) const;
            HOST_DEVICE FORCEINLINE TVector2 operator-( const TVector2<T>& other ) const;
            HOST_DEVICE FORCEINLINE TVector2 operator-( void ) const;
            HOST_DEVICE FORCEINLINE TVector2 operator*( const TVector2<T>& other ) const;
            HOST_DEVICE FORCEINLINE TVector2 operator/( const TVector2<T>& other ) const;
            HOST_DEVICE FORCEINLINE TVector2 operator*( const T& value ) const;
            HOST_DEVICE FORCEINLINE TVector2 operator/( const T& value ) const;

            HOST_DEVICE FORCEINLINE TVector2& operator+=( const TVector2& other );
            HOST_DEVICE FORCEINLINE TVector2& operator-=( const TVector2& other );
            HOST_DEVICE FORCEINLINE TVector2& operator*=( const TVector2& other );
            HOST_DEVICE FORCEINLINE TVector2& operator/=( const TVector2& other );
            HOST_DEVICE FORCEINLINE TVector2& operator*=( const T& value );
            HOST_DEVICE FORCEINLINE TVector2& operator/=( const T& value );

            template <typename R>
            HOST_DEVICE FORCEINLINE friend TVector2 operator*( R value, const TVector2& vector )
            { return vector.operator*( value ); }
            template <typename R>
            HOST_DEVICE FORCEINLINE friend TVector2 operator/( R value, const TVector2& vector )
            { return TVector2( value / vector.x, value / vector.y ); }

            template<typename R>
            explicit TVector2<T>( const TVector2<R>& other ) : TVector2<T>( (T)other.X, (T)other.Y ) {}
        };
    }
}

#include "Math/Vector2.inl"

