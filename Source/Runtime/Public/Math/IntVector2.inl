#pragma once

#include <math.h>
#include <stdexcept>

namespace EE
{
    namespace Math
    {
        template <typename T>
        FORCEINLINE TIntVector2<T>::TIntVector2()
            : x( 0 ), y( 0 )
        {
        }

        template <typename T>
        template <typename R>
        FORCEINLINE TIntVector2<T>::TIntVector2( const TVector2<R>& vector )
            : x( (T)vector.x ), y( (T)vector.y )
        {
        }

        template <typename T>
        template <typename R>
        FORCEINLINE TIntVector2<T>::TIntVector2( const TVector3<R>& vector )
            : x( (T)vector.x ), y( (T)vector.y )
        {
        }

        template <typename T>
        template <typename R>
        FORCEINLINE TIntVector2<T>::TIntVector2( const TVector4<R>& vector )
            : x( (T)vector.x ), y( (T)vector.y )
        {
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>::TIntVector2( const TIntVector2& vector )
            : x( vector.x ), y( vector.y )
        {
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>::TIntVector2( const TIntVector3<T>& vector )
            : x( vector.x ), y( vector.y )
        {
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>::TIntVector2( const T& x, const T& y )
            : x( x ), y( y )
        {
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>::TIntVector2( const T& value )
            : x( value ), y( value )
        {
        }

        template <typename T>
        template <typename R>
        inline R TIntVector2<T>::Magnitude() const
        {
            return sqrtf( x * R( x ) + y * R( y ) );
        }

        template <typename T>
        inline T TIntVector2<T>::MagnitudeSquared() const
        {
            return x * x + y * y;
        }

        template <typename T>
        FORCEINLINE HOST_DEVICE T TIntVector2<T>::Dot( const TIntVector2& other ) const
        {
            return (x * other.x) + (y * other.y);
        }

        template <typename T>
        FORCEINLINE HOST_DEVICE T TIntVector2<T>::Cross( const TIntVector2& other ) const
        {
            return x * other.y - y * other.x;
        }

        template <typename T>
        FORCEINLINE HOST_DEVICE T TIntVector2<T>::Dot( const TIntVector2& a, const TIntVector2& b )
        {
            return (a.x * b.x) + (a.y * b.y);
        }

        template <typename T>
        FORCEINLINE HOST_DEVICE T TIntVector2<T>::Cross( const TIntVector2& a, const TIntVector2& b )
        {
            return a.x * b.y - a.y * b.x;
        }

        template <typename T>
        template <typename R>
        inline TVector2<R> TIntVector2<T>::FloatVector2() const
        {
            return TVector2<R>( float( x ), float( y ) );
        }

        template <typename T>
        inline const T* TIntVector2<T>::PointerToValue() const
        {
            return &x;
        }

        template <typename T>
        inline T& TIntVector2<T>::operator[]( unsigned char i )
        {
            EE_CORE_ASSERT( i <= 1, "TIntVector2 index out of bounds" );
            return ((T*)this)[ i ];
        }

        template <typename T>
        inline T const& TIntVector2<T>::operator[]( unsigned char i ) const
        {
            EE_CORE_ASSERT( i <= 1, "TIntVector2 index out of bounds" );
            return ((T*)this)[ i ];
        }

        template <typename T>
        FORCEINLINE TIntVector2<T> TIntVector2<T>::operator * ( const TIntVector2<T>& other ) const
        {
            return TIntVector2<T>(
                x * other.x,
                y * other.y
            );
        }

        template <typename T>
        FORCEINLINE TIntVector2<T> TIntVector2<T>::operator/( const TIntVector2<T>& other ) const
        {
            return TIntVector2<T>(
                x / other.x,
                y / other.y
            );
        }

        template <typename T>
        FORCEINLINE bool TIntVector2<T>::operator==( const TIntVector2<T>& other )
        {
            return (x == other.x && y == other.y);
        }

        template <typename T>
        FORCEINLINE bool TIntVector2<T>::operator!=( const TIntVector2<T>& other )
        {
            return (x != other.x || y != other.y);
        }

        template <typename T>
        FORCEINLINE TIntVector2<T> TIntVector2<T>::operator+( const TIntVector2<T>& other ) const
        {
            return TIntVector2<T>( x + other.x, y + other.y );
        }

        template <typename T>
        FORCEINLINE TIntVector2<T> TIntVector2<T>::operator-( const TIntVector2<T>& other ) const
        {
            return TIntVector2<T>( x - other.x, y - other.y );
        }

        template <typename T>
        FORCEINLINE TIntVector2<T> TIntVector2<T>::operator-( void ) const
        {
            return TIntVector2<T>( -x, -y );
        }

        template <typename T>
        FORCEINLINE TIntVector2<T> TIntVector2<T>::operator*( const T& value ) const
        {
            return TIntVector2<T>( x * value, y * value );
        }

        template <typename T>
        FORCEINLINE TIntVector2<T> TIntVector2<T>::operator/( const T& value ) const
        {
            return TIntVector2<T>( x / value, y / value );
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>& TIntVector2<T>::operator+=( const TIntVector2<T>& other )
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>& TIntVector2<T>::operator-=( const TIntVector2<T>& other )
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>& TIntVector2<T>::operator*=( const TIntVector2<T>& other )
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>& TIntVector2<T>::operator/=( const TIntVector2<T>& other )
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>& TIntVector2<T>::operator*=( const T& value )
        {
            x *= value;
            y *= value;
            return *this;
        }

        template <typename T>
        FORCEINLINE TIntVector2<T>& TIntVector2<T>::operator/=( const T& value )
        {
            x /= value;
            y /= value;
            return *this;
        }

        template <typename T>
        inline TIntVector2<T> operator*( T value, const TIntVector2<T>& vector )
        {
            return TIntVector2<T>( value * vector.x, value * vector.y );
        }

        template <typename T>
        inline TIntVector2<T> operator/( T value, const TIntVector2<T>& vector )
        {
            return TIntVector2<T>( value / vector.x, value / vector.y );
        }
    }
}